LOCAL_PATH:= $(call my-dir)
#ָ��LOCAL_PATH���ڲ���Դ�ļ�


include $(CLEAR_VARS)
#Android.mk�п��Զ���������ģ�飬ÿ������ģ�鶼����include $(CLEAR_VARS)��ʼ
LOCAL_SRC_FILES:= \
    ws_input.c
#LOCAL_SRC_FILES�м���Դ�ļ�·��

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
#LOCAL_SHARED_LIBRARIES�м�������Ҫ���ӵĶ�̬�⣨*.so��������


LOCAL_MODULE:= libwsinput
#LOCAL_MODULE��ʾģ�����յ�����

include $(BUILD_SHARED_LIBRARY)
