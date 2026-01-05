
/******************************************************************************
 * @file    IAudioCapture.h
 * @brief   Brief description of this file
 * @author  Xie Wei
 * @date    1st Dec 2025
 * @version 1.0
 *****************************************************************************/

#pragma once
#include "capture_export.h"
#include "common.h"
#include <memory>

namespace Capture::audio {

    struct AudioInfo {
        int sample_rate;
        int channels;
    };

    /**
     * @class   MyClass
     * @brief   Brief description of MyClass
     * @details Detailed description of the class
     * @file    $fileinputname$
     * @author  Xie Wei
     * @date    $time$
     */
    class IAudioCapture {
    public:
        virtual ~IAudioCapture() = default; 


        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

		virtual bool initialize(int sampleRate, int bufferLenMS, AudioCaptureCallbackFn callback) = 0;
        

        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

        virtual bool resume() = 0;


        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

		virtual bool pause() = 0;


        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

        virtual void stop() = 0;


        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

        virtual bool isCapturing() const = 0;


        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

		virtual void clearBuffer() = 0;


        /**
         * @brief   myFunction brief description
         * @param   param1, param2 Description of parameters
         * @return  void Description of return value
         * @file    $fileinputname$
         * @author  Xie Wei
         * @date    $time$
         */

		virtual size_t getAudioData(float* buffer, int buffer_size, int ms) = 0;

        virtual AudioInfo getAudioInfo() = 0;
    };

    std::unique_ptr<IAudioCapture> createAudioCapture();
}



