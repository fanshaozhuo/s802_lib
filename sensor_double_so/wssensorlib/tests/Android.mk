LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	fszlibtest.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils libutils libui libgui libws_sensor libdl

LOCAL_MODULE:= fszlibtest

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
