#!/usr/bin/env python3
"""
Skill Editor GUI for Tetris Skill Battle
Edit skill parameters without recompiling. Saves to skills.json.
"""
import json
import os
import sys
import tkinter as tk
from tkinter import ttk, messagebox, colorchooser

SKILLS_PATH = os.path.join(os.path.dirname(__file__), '..', 'src', 'data', 'skills.json')


class SkillEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Tetris Skill Battle - Skill Editor")
        self.root.geometry("700x500")
        self.root.resizable(True, True)

        self.skills = []
        self.current_skill = None

        self._build_ui()
        self._load_skills()
        if self.skills:
            self.skill_list.selection_set(0)
            self._on_select(None)

    def _build_ui(self):
        # Main paned window
        paned = ttk.Panedwindow(self.root, orient=tk.HORIZONTAL)
        paned.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Left: skill list
        left_frame = ttk.Frame(paned)
        paned.add(left_frame, weight=1)

        ttk.Label(left_frame, text="Skills", font=("Arial", 12, "bold")).pack(pady=5)
        self.skill_list = tk.Listbox(left_frame, width=20)
        self.skill_list.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.skill_list.bind('<<ListboxSelect>>', self._on_select)

        btn_frame = ttk.Frame(left_frame)
        btn_frame.pack(fill=tk.X, padx=5, pady=5)
        ttk.Button(btn_frame, text="Save All", command=self._save_all).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Reload", command=self._load_skills).pack(side=tk.LEFT, padx=2)

        # Right: editor
        self.editor_frame = ttk.Frame(paned)
        paned.add(self.editor_frame, weight=3)

        # Form fields
        fields = [
            ("typeId", "Type ID"),
            ("name", "Name"),
            ("themeColor", "Color"),
        ]
        self.entries = {}
        row = 0
        for key, label in fields:
            ttk.Label(self.editor_frame, text=label + ":", font=("Arial", 10)).grid(
                row=row, column=0, sticky=tk.W, padx=10, pady=5)
            if key == "themeColor":
                frame = ttk.Frame(self.editor_frame)
                frame.grid(row=row, column=1, sticky=tk.EW, padx=10, pady=5)
                self.entries[key] = ttk.Entry(frame, width=10)
                self.entries[key].pack(side=tk.LEFT)
                ttk.Button(frame, text="Pick Color",
                          command=lambda: self._pick_color()).pack(side=tk.LEFT, padx=5)
                self.color_preview = tk.Canvas(frame, width=24, height=24, bg="#000000")
                self.color_preview.pack(side=tk.LEFT)
            else:
                self.entries[key] = ttk.Entry(self.editor_frame, width=40)
                self.entries[key].grid(row=row, column=1, sticky=tk.EW, padx=10, pady=5)
            row += 1

        ttk.Label(self.editor_frame, text="Description:", font=("Arial", 10)).grid(
            row=row, column=0, sticky=tk.W, padx=10, pady=5)
        self.desc_text = tk.Text(self.editor_frame, height=3, width=45)
        self.desc_text.grid(row=row, column=1, sticky=tk.EW, padx=10, pady=5)
        row += 1

        # Checkboxes
        self.is_instant_var = tk.BooleanVar()
        ttk.Checkbutton(self.editor_frame, text="Instant (no duration)",
                       variable=self.is_instant_var,
                       command=self._on_instant_toggle).grid(
            row=row, column=1, sticky=tk.W, padx=10, pady=5)
        row += 1

        # Duration and energy
        for key, label in [("duration", "Duration (ms)"), ("energyCost", "Energy Cost")]:
            ttk.Label(self.editor_frame, text=label + ":", font=("Arial", 10)).grid(
                row=row, column=0, sticky=tk.W, padx=10, pady=5)
            self.entries[key] = ttk.Entry(self.editor_frame, width=10)
            self.entries[key].grid(row=row, column=1, sticky=tk.W, padx=10, pady=5)
            row += 1

        # Apply button
        ttk.Button(self.editor_frame, text="Apply Changes",
                  command=self._apply_changes).grid(
            row=row, column=1, sticky=tk.E, padx=10, pady=15)

        self.editor_frame.columnconfigure(1, weight=1)

    def _load_skills(self):
        try:
            with open(SKILLS_PATH, 'r', encoding='utf-8') as f:
                self.skills = json.load(f)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load skills.json:\n{e}")
            self.skills = []

        self.skill_list.delete(0, tk.END)
        for s in self.skills:
            self.skill_list.insert(tk.END, s.get("name", s.get("typeId", "???")))

    def _on_select(self, event):
        sel = self.skill_list.curselection()
        if not sel:
            return
        idx = sel[0]
        if idx >= len(self.skills):
            return
        self.current_skill = self.skills[idx]
        self._populate_form(self.current_skill)

    def _populate_form(self, skill):
        self.entries["typeId"].delete(0, tk.END)
        self.entries["typeId"].insert(0, skill.get("typeId", ""))
        self.entries["name"].delete(0, tk.END)
        self.entries["name"].insert(0, skill.get("name", ""))
        self.entries["themeColor"].delete(0, tk.END)
        self.entries["themeColor"].insert(0, skill.get("themeColor", "#FFFFFF"))
        self.color_preview.configure(bg=skill.get("themeColor", "#FFFFFF"))

        self.desc_text.delete("1.0", tk.END)
        self.desc_text.insert("1.0", skill.get("description", ""))

        self.is_instant_var.set(skill.get("isInstant", False))
        self.entries["duration"].delete(0, tk.END)
        self.entries["duration"].insert(0, str(skill.get("duration", 0)))
        self.entries["energyCost"].delete(0, tk.END)
        self.entries["energyCost"].insert(0, str(skill.get("energyCost", 100)))

        self._on_instant_toggle()

    def _on_instant_toggle(self):
        if self.is_instant_var.get():
            self.entries["duration"].configure(state="disabled")
        else:
            self.entries["duration"].configure(state="normal")

    def _pick_color(self):
        color = colorchooser.askcolor(
            initialcolor=self.entries["themeColor"].get(),
            title="Pick Skill Color")
        if color and color[1]:
            self.entries["themeColor"].delete(0, tk.END)
            self.entries["themeColor"].insert(0, color[1])
            self.color_preview.configure(bg=color[1])

    def _apply_changes(self):
        if self.current_skill is None:
            return
        try:
            self.current_skill["typeId"] = self.entries["typeId"].get()
            self.current_skill["name"] = self.entries["name"].get()
            self.current_skill["themeColor"] = self.entries["themeColor"].get()
            self.current_skill["description"] = self.desc_text.get("1.0", tk.END).strip()
            self.current_skill["isInstant"] = self.is_instant_var.get()
            if not self.is_instant_var.get():
                self.current_skill["duration"] = int(self.entries["duration"].get())
            else:
                self.current_skill["duration"] = 0
            self.current_skill["energyCost"] = int(self.entries["energyCost"].get())

            # Update list display
            sel = self.skill_list.curselection()
            if sel:
                self.skill_list.delete(sel[0])
                self.skill_list.insert(sel[0], self.current_skill.get("name", "???"))
                self.skill_list.selection_set(sel[0])

            messagebox.showinfo("Success", "Changes applied. Save to persist.")
        except ValueError as e:
            messagebox.showerror("Error", f"Invalid value: {e}")

    def _save_all(self):
        try:
            with open(SKILLS_PATH, 'w', encoding='utf-8') as f:
                json.dump(self.skills, f, indent=4, ensure_ascii=False)
            messagebox.showinfo("Saved", f"Skills saved to:\n{SKILLS_PATH}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save:\n{e}")


def main():
    root = tk.Tk()
    app = SkillEditor(root)
    root.mainloop()


if __name__ == "__main__":
    main()
