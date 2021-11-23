/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     Zhang Qipeng <zhangqipeng@uniontech.com>
*
* Maintainer: Zhang Qipeng <zhangqipeng@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AUTHMODULE_H
#define AUTHMODULE_H

#include "constants.h"

#include <DLabel>

#include <QWidget>
#include <QDebug>
#include <QPointer>

#define CAPS_LOCK QStringLiteral(":/misc/images/caps_lock.svg")
#define LOGIN_CHECK QStringLiteral(":/misc/images/login_check.svg")
#define LOGIN_LOCK QStringLiteral(":/misc/images/login_lock.svg")
#define LOGIN_WAIT QStringLiteral(":/misc/images/login_wait.svg")
#define LOGIN_RETRY QStringLiteral(":/misc/images/retry16.svg")
#define LOGIN_SPINNER QStringLiteral(":/misc/images/login_spinner.svg")
#define PASSWORD_HINT QStringLiteral(":/misc/images/password_hint.svg")
#define AUTH_LOCK QStringLiteral(":/misc/images/unlock/unlock_1.svg")
#define UnionID_Auth QStringLiteral(":/misc/images/auth/UnionID.svg")
#define ResetPassword_Exe_Path QStringLiteral("/usr/lib/dde-control-center/reset-password-dialog")

DWIDGET_USE_NAMESPACE
using namespace DDESESSIONCC;

struct LimitsInfo {
    bool locked;        // 认证锁定状态 --- true: 锁定  false: 解锁
    uint maxTries;      // 最大重试次数
    uint numFailures;   // 失败次数，一直累加
    uint unlockSecs;    // 本次锁定总解锁时间（秒），不会随着时间推移减少
    QString unlockTime; // 解锁时间（本地时间）

    void operator=(const LimitsInfo &info);

#ifdef QT_DEBUG
    friend QDebug operator<<(QDebug out, const LimitsInfo& info)
    {
        out << "locked:" << info.locked
            << ", maxTried:" << info.maxTries
            << ", numFailures:" << info.numFailures
            << ", unlockSecs:" << info.unlockSecs
            << ", unlockTime:" << info.unlockTime;
        return out;
    }
#endif
};

class AuthModule : public QWidget
{
    Q_OBJECT
public:
    enum TextType {
        AlertText,
        InputText,
        PlaceHolderText
    };
    Q_ENUM(TextType)

    explicit AuthModule(const int type, QWidget *parent = nullptr);
    ~AuthModule() override;

    inline int authType() const { return m_type; }
    inline int authStatus() const { return m_status; }

    virtual void setAnimationStatus(const bool start);
    virtual void setAuthStatus(const int status, const QString &result);
    void setAuthStatusStyle(const QString &path);
    virtual void setLimitsInfo(const LimitsInfo &info);
    void setShowAuthStatus(bool showAuthStatus);
    void setAuthStatueVisible(bool visible);
    void setAuthStatusLabel(DLabel *label);
    virtual void setAuthFactorType(AuthFactorType authFactorType);
    inline bool isMFA() const { return m_authFactorType == DDESESSIONCC::MultiAuthFactor; }

signals:
    void activeAuth(const int);
    void authFinished(const int);
    void requestAuthenticate();
    void unlockTimeChanged();

protected:
    void initConnections();
    virtual void doAnimation() { }
    virtual void updateUnlockPrompt();
    void updateUnlockTime();
    void updateIntegerMinutes();

protected:
    int m_inputType;          // 认证信息输入设备类型
    int m_status;             // 认证状态
    int m_type;               // 认证类型
    bool m_showPrompt;        // 是否显示默认提示文案
    uint m_integerMinutes;    // 认证剩余解锁的整数分钟
    LimitsInfo *m_limitsInfo; // 认证限制相关信息
    QPointer<DLabel> m_authStatusLabel;     // 认证状态图标
    QTimer *m_aniTimer;       // 动画执行定时器
    QTimer *m_unlockTimer;    // 认证解锁定时器
    QTimer *m_unlockTimerTmp; // 认证解锁定时器
    bool m_showAuthStatus;    // 是否显示认证状态
    bool m_isAuthing;         // 是否正在验证
    AuthFactorType m_authFactorType;    // 验证因子类型
};

#endif // AUTHMODULE_H
