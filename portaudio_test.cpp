#include <iostream>
#include <vector>
#include <portaudio.h>
#include <fftw3.h>

// Define constants for audio processing
constexpr int SAMPLE_RATE = 44100; // Standard CD-quality sample rate
constexpr int FRAMES_PER_BUFFER = 512; // Size of the audio buffer

// Define a structure for our user data
struct PAUserData {
    float sinePhase;
    fftw_plan fftPlan;
    double* fftInput;
    double* fftOutput;
};

// This is the audio callback function, called by PortAudio when it needs audio processing
static int audioCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData) {
    // Cast userData to our custom structure
    auto* user = static_cast<PAUserData*>(userData);
    auto* out = static_cast<float*>(outputBuffer);
    const auto* in = static_cast<const float*>(inputBuffer);
    
    // Prevent unused variable warnings (in case they are not used later)
    (void) timeInfo;
    (void) statusFlags;
    
    if (inputBuffer == nullptr) {
        for(unsigned i = 0; i<framesPerBuffer; i++) {
            // If there is no input, output silence
            *out++ = 0;
        }
    } else {
        for(unsigned i = 0; i<framesPerBuffer; i++) {
            // Copy input to output
            *out++ = *in++;
            
            // Fill fftInput with the audio sample
            user->fftInput[i] = static_cast<float>(*in);
        }
        
        // Execute FFT
        fftw_execute(user->fftPlan);
        
        // ... Here, we could do something with the fftOutput data, like filtering
        
        // Example: zero out all FFT bins (this would mute the audio)
        for(int i = 0; i<framesPerBuffer; i++) {
            user->fftOutput[i] = 0;
        }
        
        // Reverse FFT would go here
    }
    return paContinue;
}

int main() {
    PaError err;
    PAUserData userData;
    PaStream* stream;

    // Initialize user data
    userData.sinePhase = 0;
    userData.fftInput = fftw_alloc_real(FRAMES_PER_BUFFER); 
    userData.fftOutput = fftw_alloc_real(FRAMES_PER_BUFFER);
    userData.fftPlan = fftw_plan_r2r_1d(FRAMES_PER_BUFFER, userData.fftInput, userData.fftOutput, FFTW_R2HC, FFTW_ESTIMATE);

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError) std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;

    // Open an audio I/O stream
    err = Pa_OpenDefaultStream(&stream,
                               1,          // mono input
                               1,          // mono output
                               paFloat32,  // 32 bit floating point output
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               audioCallback,
                               &userData);
    
    if (err != paNoError) std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;

    // Start the audio I/O stream
    err = Pa_StartStream(stream);
    if (err != paNoError) std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;

    // This is a placeholder for whatever processing you need to do
    std::cout << "Press Enter to stop the audio stream";
    std::cin.get();
    
    // Close the stream
    err = Pa_CloseStream(stream);
    if (err != paNoError) std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    
    // Terminate PortAudio
    Pa_Terminate();
    
    // Free FFT resources
    fftw_destroy_plan(userData.fftPlan);
    fftw_free(userData.fftInput);
    fftw_free(userData.fftOutput);
    
    std::cout << "Stream stopped and resources cleaned up" << std::endl;
    
    return 0;
}

