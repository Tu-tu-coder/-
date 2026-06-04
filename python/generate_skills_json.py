#!/usr/bin/env python3
"""
Generate skills.json from a CSV file.
CSV format: typeId,name,description,themeColor,isInstant,duration,energyCost
"""
import csv
import json
import os
import sys

SKILLS_PATH = os.path.join(os.path.dirname(__file__), '..', 'src', 'data', 'skills.json')


def csv_to_json(csv_path, output_path=None):
    if output_path is None:
        output_path = SKILLS_PATH

    skills = []
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            skill = {
                "typeId": row.get("typeId", "").strip(),
                "name": row.get("name", "").strip(),
                "description": row.get("description", "").strip(),
                "themeColor": row.get("themeColor", "#FFFFFF").strip(),
                "isInstant": row.get("isInstant", "false").strip().lower() == "true",
                "duration": int(row.get("duration", "0")),
                "energyCost": int(row.get("energyCost", "100")),
            }
            skills.append(skill)

    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(skills, f, indent=4, ensure_ascii=False)

    print(f"Generated {len(skills)} skills → {output_path}")


def json_to_csv(output_path=None):
    if output_path is None:
        output_path = "skills_export.csv"

    with open(SKILLS_PATH, 'r', encoding='utf-8') as f:
        skills = json.load(f)

    with open(output_path, 'w', encoding='utf-8', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=[
            "typeId", "name", "description", "themeColor",
            "isInstant", "duration", "energyCost"
        ])
        writer.writeheader()
        for s in skills:
            writer.writerow({
                "typeId": s.get("typeId", ""),
                "name": s.get("name", ""),
                "description": s.get("description", ""),
                "themeColor": s.get("themeColor", ""),
                "isInstant": str(s.get("isInstant", False)).lower(),
                "duration": s.get("duration", 0),
                "energyCost": s.get("energyCost", 100),
            })

    print(f"Exported {len(skills)} skills → {output_path}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python generate_skills_json.py import <input.csv> [output.json]")
        print("  python generate_skills_json.py export [output.csv]")
        sys.exit(1)

    cmd = sys.argv[1]
    if cmd == "import":
        csv_path = sys.argv[2]
        out_path = sys.argv[3] if len(sys.argv) > 3 else None
        csv_to_json(csv_path, out_path)
    elif cmd == "export":
        out_path = sys.argv[2] if len(sys.argv) > 2 else None
        json_to_csv(out_path)
    else:
        print(f"Unknown command: {cmd}")
        sys.exit(1)
