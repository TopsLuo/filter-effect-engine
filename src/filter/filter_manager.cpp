//
// Created by zhouhaibo on 2017/9/15.
//

#include <utils/parse_config.h>
#include "utils/parse_config.h"
#include "filter_manager.h"

FilterManager::FilterManager() {
    mIsInitialized = false;
    mFboTextureID = 0;
    mFboBufferID = 0;
    mCurrentFilter = No_Filter;
    mMultiInputFilter = NULL;
    mFilterConfig = NULL;
}

FilterManager::~FilterManager() {
    if (mIsInitialized) {
        destroy();
    }
}

void FilterManager::init(int width, int height) {

    if (mFboTextureID == 0) {
        glGenTextures(1, &mFboTextureID);
    }

    if (mFboBufferID == 0) {
        glGenFramebuffers(1, &mFboBufferID);
    }

    bindFrameBuffer(mFboTextureID, mFboBufferID, width, height);

    mIsInitialized = true;
}

void FilterManager::setFilter(ConstString zipPath) {
    if (!mIsInitialized || NULL == zipPath) {
        return;
    }
    // 解析
    if (NULL != mFilterConfig) {
        if (NULL != mFilterConfig->data) {
            delete mFilterConfig->data;
            mFilterConfig->data = NULL;
        }
        delete mFilterConfig;
        mFilterConfig = NULL;
    }

    mFilterConfig = new FilterConfig;
    parsingFilterConfig(zipPath, mFilterConfig);
}


GLuint FilterManager::process(GLuint textureId, int width, int height) {
    if (!mIsInitialized) {
        return textureId;
    }

    // 预处理
    if(mFilterConfig->type <= 0) {
        return textureId;
    }

    FilterType type = (FilterType) mFilterConfig->type;
    // 需要重新初始化Filter
    if (mCurrentFilter != type) {
        if (NULL != mMultiInputFilter) {
            mMultiInputFilter->destroy();
            delete mMultiInputFilter;
            mMultiInputFilter = NULL;
        }

        FilterFactory *factory = new FilterFactory();
        mMultiInputFilter = factory->createFilter(type);
        mCurrentFilter = type;
        delete factory;
        mMultiInputFilter->init(mFilterConfig);
    }

    GLuint targetTexture = textureId;
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, mFboBufferID);

    if (NULL != mMultiInputFilter && mCurrentFilter != No_Filter) {
        mMultiInputFilter->process(targetTexture, width, height);
        targetTexture = mFboTextureID;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return targetTexture;
}

void FilterManager::destroy() {
    if (!mIsInitialized) {
        LOGE("BeautifyInterFace is not Initialized");
        return;
    }

    if (NULL != mMultiInputFilter) {
        mMultiInputFilter->destroy();
        delete mMultiInputFilter;
    }

    if (mFboTextureID > 0) {
        glDeleteTextures(1, &mFboTextureID);
    }

    if (mFboBufferID > 0) {
        glDeleteFramebuffers(1, &mFboBufferID);
    }

    mCurrentFilter = No_Filter;

    mIsInitialized = false;
}

void FilterManager::getResult(unsigned char *buffer) {
    if (NULL != mMultiInputFilter) {
//        mMultiInputFilter->getResult(buffer);
    }
}