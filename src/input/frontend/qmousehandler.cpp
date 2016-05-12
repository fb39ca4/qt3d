/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmousehandler.h"
#include "qmousehandler_p.h"
#include "qmousedevice.h"
#include "qmouseevent.h"
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
/*! \internal */
QMouseHandlerPrivate::QMouseHandlerPrivate()
    : QComponentPrivate()
    , m_mouseDevice(nullptr)
    , m_containsMouse(false)
{
    m_shareable = false;
}

void QMouseHandlerPrivate::mouseEvent(QMouseEvent *event)
{
    Q_Q(QMouseHandler);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (event->wasHeld())
            emit q->pressAndHold(event);
        else
            emit q->pressed(event);
        break;
    case QEvent::MouseButtonRelease:
        emit q->released(event);
        break;
    case Qt::TapGesture:
        emit q->clicked(event);
        break;
    case QEvent::MouseButtonDblClick:
        emit q->doubleClicked(event);
        break;
    case QEvent::MouseMove:
        emit q->positionChanged(event);
        break;
    default:
        break;
    }
}

/*!
 * \qmltype MouseHandler
 * \instantiates Qt3DInput::QMouseHandler
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \brief Provides mouse event notification
 *
 * \TODO
 * \sa MouseDevice
 */

/*!
 * \class Qt3DInput::QMouseHandler
 * \inmodule Qt3DInput
 *
 * \brief Provides a means of being notified about mouse events when attached to
 * a QMouseDevice instance.
 *
 * \since 5.5
 *
 * \note QMouseHandler components shouldn't be shared, not respecting that
 * condition will most likely result in undefined behaviors.
 *
 * \sa QMouseDevice
 */

/*!
 * Constructs a new QMouseHandler instance with parent \a parent.
 */
QMouseHandler::QMouseHandler(QNode *parent)
    : QComponent(*new QMouseHandlerPrivate, parent)
{
}

/*!
 * Sets the mouse device of the QMouseHandler instance to \a mouseDevice.
 */
void QMouseHandler::setSourceDevice(QMouseDevice *mouseDevice)
{
    Q_D(QMouseHandler);
    if (d->m_mouseDevice != mouseDevice) {
        d->m_mouseDevice = mouseDevice;
        emit sourceDeviceChanged(mouseDevice);
    }
}

/*!
 * Returns the current mouse source device of the QMouseHandler instance.
 */
QMouseDevice *QMouseHandler::sourceDevice() const
{
    Q_D(const QMouseHandler);
    return d->m_mouseDevice;
}

/*!
 * Returns \c true if the QMouseHandler currently contains the mouse.
 *
 * \note In this context, contains mean that the ray originating from the
 * mouse is intersecting with the Qt3DCore::QEntity that aggregates the current
 * QMouseHandler instance component.
 */
bool QMouseHandler::containsMouse() const
{
    Q_D(const QMouseHandler);
    return d->m_containsMouse;
}

void QMouseHandler::setContainsMouse(bool contains)
{
    Q_D(QMouseHandler);
    if (contains != d->m_containsMouse) {
        d->m_containsMouse = contains;
        emit containsMouseChanged(contains);
    }
}

void QMouseHandler::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Q_D(QMouseHandler);
    QPropertyUpdatedChangePtr e = qSharedPointerCast<QPropertyUpdatedChange>(change);
    if (e->type() == PropertyUpdated) {
        if (e->propertyName() == QByteArrayLiteral("mouse")) {
            QMouseEventPtr ev = e->value().value<QMouseEventPtr>();
            d->mouseEvent(ev.data());
        } else if (e->propertyName() == QByteArrayLiteral("wheel")) {
            QWheelEventPtr ev = e->value().value<QWheelEventPtr>();
            emit wheel(ev.data());
        }
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QMouseHandler::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QMouseHandlerData>::create(this);
    auto &data = creationChange->data;

    Q_D(const QMouseHandler);
    data.mouseDeviceId = qIdForNode(d->m_mouseDevice);

    return creationChange;
}

} // namespace Qt3DInput

QT_END_NAMESPACE
