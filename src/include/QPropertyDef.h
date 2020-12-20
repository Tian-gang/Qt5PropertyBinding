#pragma once

#include <QtGlobal>

#if defined(Q_OS_WIN)
#define QPROPERTY_BINDING_DECL_EXPORT __declspec(dllexport)
#define QPROPERTY_BINDING_DECL_IMPORT __declspec(dllimport)
#else
#define QPROPERTY_BINDING_DECL_EXPORT __attribute__((visibility("default")))
#define QPROPERTY_BINDING_DECL_IMPORT __attribute__((visibility("default")))
#define QPROPERTY_BINDING_DECL_HIDDEN __attribute__((visibility("hidden")))
#endif

#if defined(BUILD_STATIC)
#define QPROPERTY_BINDING_EXPORT
#else
#if defined(QPROPERTY_BINDING_LIBRARY)
#define QPROPERTY_BINDING_EXPORT QPROPERTY_BINDING_DECL_EXPORT
#else
#define QPROPERTY_BINDING_EXPORT QPROPERTY_BINDING_DECL_IMPORT
#endif
#endif
