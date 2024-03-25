let audioCtx

function renderScreen() {
    const getPixels = Module.cwrap('getPixels', 'int', []);
    const ctx = document.querySelector('canvas').getContext('2d');
    const pointer = getPixels();
    const width = 336;
    const height = 192;
    // const data = Module.HEAPU8.subarray(pointer, width * height * 4);
    const data = new Uint8ClampedArray(Module.HEAPU8.buffer, pointer, width * height * 4);
    // console.log(data);
    const img = new ImageData(data, width, height);
    ctx.putImageData(img, 0, 0);

    playAudioBuffer()

    requestAnimationFrame(renderScreen);
}

function playAudioBuffer() {
    const getSoundBuffer = Module.cwrap('getSoundBuffer', 'int', [])
    const getSoundBufferLen = Module.cwrap('getSoundBufferLen', 'int', [])
    const getSoundBufferAllocatedSize = Module.cwrap('getSoundBufferAllocatedSize', 'int', [])
    const getSoundFrequency = Module.cwrap('getSoundFrequency', 'int', [])
    const getNumSoundChannels = Module.cwrap('getNumSoundChannels', 'int', [])
    const getSoundSampleSize = Module.cwrap('getSoundSampleSize', 'int', [])

    const ptr = getSoundBuffer()
    const bufferLen = getSoundBufferLen()

    if (bufferLen === 0) return

    // console.log({
    //     getSoundBufferLen: getSoundBufferLen(),
    //     getSoundBufferAllocatedSize: getSoundBufferAllocatedSize(),
    //     getSoundFrequency: getSoundFrequency(),
    //     getNumSoundChannels: getNumSoundChannels(),
    //     getSoundSampleSize: getSoundSampleSize(),
    // })

    const data = new Uint8Array(Module.HEAPU8.buffer, ptr, getSoundBufferAllocatedSize())

    const buffer = new AudioBuffer({
        numberOfChannels: getNumSoundChannels(),
        length: bufferLen,
        sampleRate: getSoundFrequency(),
    })

    // buffer.copyToChannel(data, 0, 0)

    for (let c = 0; c < buffer.numberOfChannels; c++) {
      const chanBuffer = buffer.getChannelData(c);
      for (let i = 0; i < buffer.length; i++) {
        chanBuffer[i] = data[i];
      }
    }

    const source = audioCtx.createBufferSource()
    source.buffer = buffer
    source.connect(audioCtx.destination)
    source.start()
}

// function renderScreen() {
//     const ctx = document.querySelector('canvas').getContext('2d');
//     const pointer = instance.exports._render();
//     const data = new Uint8ClampedArray(memory.buffer, pointer, width * height * 4);
//     const img = new ImageData(data, width, height);
//     ctx.putImageData(img, 0, 0);
// }

function resize() {
    const canvas = document.querySelector("canvas");
    const canvasRatio = canvas.height / canvas.width;
    const windowRatio = window.innerHeight / window.innerWidth;
    let width;
    let height;

    if (windowRatio < canvasRatio) {
        height = window.innerHeight;
        width = height / canvasRatio;
    } else {
        width = window.innerWidth;
        height = width * canvasRatio;
    }

    canvas.style.width = `${width}px`;
    canvas.style.height = `${height}px`;
}


function setup() {
    const toHide = ["#output", "a", "#status", "#controls"];
    toHide.forEach(i => document.querySelector(i).style.display = "none");

    audioCtx = new AudioContext()
}

// window.addEventListener('resize', resize, false);

// Module.postRun = [setup, resize, renderScreen];