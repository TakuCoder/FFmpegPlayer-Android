//
// Created by saite on 2019/8/9.
//

#ifndef ANDROIDPROJECT_LOG_H
#define ANDROIDPROJECT_LOG_H
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "will_E", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "will_I", __VA_ARGS__))
#endif //ANDROIDPROJECT_LOG_H
