#ifndef SKILLPANELWIDGET_H
#define SKILLPANELWIDGET_H

#include <QWidget>
#include <QLabel>

class SkillManager;

class SkillPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit SkillPanelWidget(SkillManager* skillMgr, QWidget* parent = nullptr);

    void updateSkill();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    SkillManager* m_skillMgr = nullptr;
    QLabel* m_nameLabel;
    QLabel* m_descLabel;
    QLabel* m_keyHintLabel;
};

#endif
