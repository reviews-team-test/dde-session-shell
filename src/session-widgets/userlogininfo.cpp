/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lixin <lixin_cm@deepin.com>
 *
 * Maintainer: lixin <lixin_cm@deepin.com>
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

#include "userlogininfo.h"
#include "userinfo.h"
#include "userloginwidget.h"
#include "sessionbasemodel.h"
#include "userframelist.h"
#include "src/global_util/constants.h"

UserLoginInfo::UserLoginInfo(SessionBaseModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_userLoginWidget(new UserLoginWidget)
    , m_userFrameList(new UserFrameList)
{
    m_userLoginWidget->setWidgetWidth(DDESESSIONCC::PASSWDLINEEIDT_WIDTH);
    m_userFrameList->setModel(model);
    initConnect();
}

void UserLoginInfo::setUser(std::shared_ptr<User> user)
{
    for (auto connect : m_currentUserConnects) {
        m_user->disconnect(connect);
    }

    m_currentUserConnects.clear();
    m_currentUserConnects << connect(user.get(), &User::lockChanged, this, &UserLoginInfo::userLockChanged);
    m_currentUserConnects << connect(user.get(), &User::avatarChanged, m_userLoginWidget, &UserLoginWidget::setAvatar);
    m_currentUserConnects << connect(user.get(), &User::displayNameChanged, m_userLoginWidget, &UserLoginWidget::setName);
    m_currentUserConnects << connect(user.get(), &User::kbLayoutListChanged, m_userLoginWidget, &UserLoginWidget::updateKBLayout, Qt::UniqueConnection);
    m_currentUserConnects << connect(user.get(), &User::currentKBLayoutChanged, m_userLoginWidget, &UserLoginWidget::setDefaultKBLayout, Qt::UniqueConnection);

    m_user = user;

    m_userLoginWidget->setName(user->displayName());
    m_userLoginWidget->setAvatar(user->avatarPath());
    m_userLoginWidget->setUserAvatarSize(UserLoginWidget::AvatarLargeSize);
    m_userLoginWidget->updateAuthType(m_model->currentType());
    m_userLoginWidget->disablePassword(user.get()->isLock(), user->lockNum());
    m_userLoginWidget->setKBLayoutList(user->kbLayoutList());
    m_userLoginWidget->setDefKBLayout(user->currentKBLayout());

    if (m_user->isNoPasswdGrp()) {
        m_userLoginWidget->setWidgetShowType(UserLoginWidget::NoPasswordType);
    } else {
        m_userLoginWidget->setWidgetShowType(UserLoginWidget::NormalType);
    }
}

void UserLoginInfo::initConnect()
{
    //UserLoginWidget
    connect(m_userLoginWidget, &UserLoginWidget::requestAuthUser, this, &UserLoginInfo::requestAuthUser);
    connect(m_model, &SessionBaseModel::authFaildMessage, m_userLoginWidget, &UserLoginWidget::setFaildMessage);
    connect(m_model, &SessionBaseModel::authFaildTipsMessage, m_userLoginWidget, &UserLoginWidget::setFaildTipMessage);
    connect(m_model, &SessionBaseModel::authFinished, this, [ = ](bool success) {
        if (success) {
            m_userLoginWidget->resetAllState();
        }
    });
    connect(m_userLoginWidget, &UserLoginWidget::requestUserKBLayoutChanged, this, [ = ](const QString & value) {
        emit requestSetLayout(m_user, value);
    });

    //UserFrameList
    connect(m_userFrameList, &UserFrameList::clicked, this, &UserLoginInfo::hideUserFrameList);
    connect(m_userFrameList, &UserFrameList::requestSwitchUser, this, &UserLoginInfo::requestSwitchUser);
}

UserLoginWidget *UserLoginInfo::getUserLoginWidget()
{
    return m_userLoginWidget;
}

UserFrameList *UserLoginInfo::getUserFrameList()
{
    return m_userFrameList;
}

void UserLoginInfo::hideKBLayout()
{
    m_userLoginWidget->hideKBLayout();
}

void UserLoginInfo::userLockChanged(bool disable)
{
    m_userLoginWidget->disablePassword(disable, m_user->lockNum());
}