LOCAL_PATH:= $(call my-dir)
#指定LOCAL_PATH用于查找源文件


include $(CLEAR_VARS)
#Android.mk中可以定义多个编译模块，每个编译模块都是以include $(CLEAR_VARS)开始
LOCAL_SRC_FILES:= \
    ws_input.c
#LOCAL_SRC_FILES中加入源文件路径

LOCAL_CFLAGS:= -DLOG_TAG=\"WSInputlib\"

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libhardware \
	libutils \
	libbinder \
	libui \
	libgui \
	libc \
	libm
#LOCAL_SHARED_LIBRARIES中加入所需要链接的动态库（*.so）的名称


LOCAL_MODULE:= libwsinput
#LOCAL_MODULE表示模块最终的名称

include $(BUILD_SHARED_LIBRARY)
