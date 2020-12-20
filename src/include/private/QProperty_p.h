/***************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QPROPERTY_P_H
#define QPROPERTY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include <qglobal.h>
#include <qscopedpointer.h>

#include <array>
#include <vector>

#include "../QProperty.h"

// Keep all classes related to QProperty in one compilation unit. Performance of this code is crucial and
// we need to allow the compiler to inline where it makes sense.

// This is a helper "namespace"
struct QPropertyBindingDataPointer
{
    const QtPrivate::QPropertyBindingData* ptr = nullptr;

    QPropertyBindingPrivate* bindingPtr() const
    {
        if (ptr->d_ptr & QtPrivate::QPropertyBindingData::BindingBit)
            return reinterpret_cast<QPropertyBindingPrivate*>(ptr->d_ptr - QtPrivate::QPropertyBindingData::BindingBit);
        return nullptr;
    }

    void setObservers(QPropertyObserver* observer)
    {
        observer->prev = reinterpret_cast<QPropertyObserver**>(&(ptr->d_ptr));
        ptr->d_ptr = reinterpret_cast<quintptr>(observer);
    }
    void fixupFirstObserverAfterMove() const;
    void addObserver(QPropertyObserver* observer);
    void setFirstObserver(QPropertyObserver* observer);
    QPropertyObserverPointer firstObserver() const;

    int observerCount() const;

    template <typename T>
    static QPropertyBindingDataPointer get(QProperty<T>& property)
    {
        return QPropertyBindingDataPointer{&property.bindingData()};
    }
};

// This is a helper "namespace"
struct QPropertyObserverPointer
{
    QPropertyObserver* ptr = nullptr;

    void unlink();

    void setBindingToMarkDirty(QPropertyBindingPrivate* binding);
    void setChangeHandler(QPropertyObserver::ChangeHandler changeHandler);
    void setAliasedProperty(QUntypedPropertyData* propertyPtr);

    void notify(QPropertyBindingPrivate* triggeringBinding, QUntypedPropertyData* propertyDataPtr,
                const bool alreadyKnownToHaveChanged = false);
    void observeProperty(QPropertyBindingDataPointer property);

    explicit operator bool() const { return ptr != nullptr; }

    QPropertyObserverPointer nextObserver() const { return {ptr->next.data()}; }
};

class QPropertyBindingErrorPrivate : public QSharedData
{
   public:
    QPropertyBindingError::Type type = QPropertyBindingError::NoError;
    QString description;
};

namespace QtPrivate
{
struct BindingEvaluationState
{
    BindingEvaluationState(QPropertyBindingPrivate* binding, QBindingStatus* status = nullptr);
    ~BindingEvaluationState() { *currentState = previousState; }

    QPropertyBindingPrivate* binding;
    BindingEvaluationState* previousState = nullptr;
    BindingEvaluationState** currentState = nullptr;
};

struct CurrentCompatProperty
{
    CurrentCompatProperty(QBindingStatus* status, QUntypedPropertyData* property);
    ~CurrentCompatProperty() { *currentState = previousState; }
    QUntypedPropertyData* property;
    CurrentCompatProperty* previousState = nullptr;
    CurrentCompatProperty** currentState = nullptr;
};

}  // namespace QtPrivate

class QPropertyBindingPrivate : public QtPrivate::RefCounted
{
   private:
    friend struct QPropertyBindingDataPointer;
    friend class QPropertyBindingPrivatePtr;

    using ObserverArray = std::array<QPropertyObserver, 4>;

   private:
    // a dependent property has changed, and the binding needs to be reevaluated on access
    bool dirty = false;
    // used to detect binding loops for lazy evaluated properties
    bool updating = false;
    bool hasStaticObserver = false;
    bool hasBindingWrapper : 1;
    // used to detect binding loops for eagerly evaluated properties
    bool eagerlyUpdating : 1;

    const QtPrivate::BindingFunctionVTable* vtable;

    union
    {
        QtPrivate::QPropertyObserverCallback staticObserverCallback = nullptr;
        QtPrivate::QPropertyBindingWrapper staticBindingWrapper;
    };
    ObserverArray inlineDependencyObservers;

    QPropertyObserverPointer firstObserver;
    QScopedPointer<std::vector<QPropertyObserver>> heapObservers;

    QUntypedPropertyData* propertyDataPtr = nullptr;

    QPropertyBindingSourceLocation location;
    QPropertyBindingError error;

    int metaTypeId{0};

   public:
    static constexpr size_t getSizeEnsuringAlignment()
    {
        constexpr auto align = alignof(std::max_align_t) - 1;
        constexpr size_t sizeEnsuringAlignment = (sizeof(QPropertyBindingPrivate) + align) & ~align;
        static_assert(sizeEnsuringAlignment % alignof(std::max_align_t) == 0,
                      "Required for placement new'ing the function behind it.");
        return sizeEnsuringAlignment;
    }

    // public because the auto-tests access it, too.
    size_t dependencyObserverCount = 0;

    QPropertyBindingPrivate(int metaTypeId, const QtPrivate::BindingFunctionVTable* vtable,
                            const QPropertyBindingSourceLocation& location)
        : hasBindingWrapper(false),
          eagerlyUpdating(false),
          vtable(vtable),
          inlineDependencyObservers()  // Explicit initialization required because of union
          ,
          location(location),
          metaTypeId(metaTypeId)
    {
    }
    ~QPropertyBindingPrivate();

    void setDirty(bool d) { dirty = d; }
    void setProperty(QUntypedPropertyData* propertyPtr) { propertyDataPtr = propertyPtr; }
    void setStaticObserver(QtPrivate::QPropertyObserverCallback callback, QtPrivate::QPropertyBindingWrapper bindingWrapper)
    {
        Q_ASSERT(!(callback && bindingWrapper));
        if (callback)
        {
            hasStaticObserver = true;
            hasBindingWrapper = false;
            staticObserverCallback = callback;
        }
        else if (bindingWrapper)
        {
            hasStaticObserver = false;
            hasBindingWrapper = true;
            staticBindingWrapper = bindingWrapper;
        }
        else
        {
            hasStaticObserver = false;
            hasBindingWrapper = false;
            staticObserverCallback = nullptr;
        }
    }
    void prependObserver(QPropertyObserverPointer observer)
    {
        observer.ptr->prev = const_cast<QPropertyObserver**>(&firstObserver.ptr);
        firstObserver = observer;
    }

    QPropertyObserverPointer takeObservers()
    {
        auto observers = firstObserver;
        firstObserver.ptr = nullptr;
        return observers;
    }

    void clearDependencyObservers()
    {
        for (size_t i = 0; i < qMin(dependencyObserverCount, inlineDependencyObservers.size()); ++i)
        {
            QPropertyObserverPointer p{&inlineDependencyObservers[i]};
            p.unlink();
        }
        if (heapObservers) heapObservers->clear();
        dependencyObserverCount = 0;
    }
    QPropertyObserverPointer allocateDependencyObserver()
    {
        if (dependencyObserverCount < inlineDependencyObservers.size())
        {
            ++dependencyObserverCount;
            return {&inlineDependencyObservers[dependencyObserverCount - 1]};
        }
        ++dependencyObserverCount;
        if (!heapObservers) heapObservers.reset(new std::vector<QPropertyObserver>());
        return {&heapObservers->emplace_back()};
    }

    QPropertyBindingSourceLocation sourceLocation() const { return location; }
    QPropertyBindingError bindingError() const { return error; }
    int valueMetaTypeId() const { return metaTypeId; }

    void unlinkAndDeref();

    void markDirtyAndNotifyObservers();
    bool evaluateIfDirtyAndReturnTrueIfValueChanged(const QUntypedPropertyData* data, QBindingStatus* status = nullptr)
    {
        if (!dirty) return false;
        return evaluateIfDirtyAndReturnTrueIfValueChanged_helper(data, status);
    }

    static QPropertyBindingPrivate* get(const QUntypedPropertyBinding& binding)
    {
        return static_cast<QPropertyBindingPrivate*>(binding.d.data());
    }

    void setError(QPropertyBindingError&& e) { error = std::move(e); }

    void detachFromProperty()
    {
        hasStaticObserver = false;
        hasBindingWrapper = false;
        propertyDataPtr = nullptr;
        clearDependencyObservers();
    }

    bool requiresEagerEvaluation() const { return hasBindingWrapper; }

    static QPropertyBindingPrivate* currentlyEvaluatingBinding();

    static void destroyAndFreeMemory(QPropertyBindingPrivate* priv)
    {
        if (priv->vtable->size == 0)
        {
            // special hack for QQmlPropertyBinding which has a
            // different memory layout than normal QPropertyBindings
            priv->vtable->destroy(priv);
        }
        else
        {
            priv->~QPropertyBindingPrivate();
            delete[] reinterpret_cast<std::byte*>(priv);
        }
    }

   private:
    bool evaluateIfDirtyAndReturnTrueIfValueChanged_helper(const QUntypedPropertyData* data, QBindingStatus* status = nullptr);
};

inline void QPropertyBindingDataPointer::setFirstObserver(QPropertyObserver* observer)
{
    if (auto* binding = bindingPtr())
    {
        binding->firstObserver.ptr = observer;
        return;
    }
    ptr->d_ptr = reinterpret_cast<quintptr>(observer);
}

inline void QPropertyBindingDataPointer::fixupFirstObserverAfterMove() const
{
    // If QPropertyBindingData has been moved, and it has an observer
    // we have to adjust the firstObesrver's prev pointer to point to
    // the moved to QPropertyBindingData's d_ptr
    if (ptr->d_ptr & QtPrivate::QPropertyBindingData::BindingBit)
        return;  // nothing to do if the observer is stored in the binding
    if (auto observer = firstObserver()) observer.ptr->prev = reinterpret_cast<QPropertyObserver**>(&(ptr->d_ptr));
}

inline QPropertyObserverPointer QPropertyBindingDataPointer::firstObserver() const
{
    if (auto* binding = bindingPtr()) return binding->firstObserver;
    return {reinterpret_cast<QPropertyObserver*>(ptr->d_ptr)};
}

#endif  // QPROPERTY_P_H
