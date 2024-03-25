class Atari800 {
  constructor(canvas) {
    this.canvas = canvas
    this.width = 336
    this.height = 192
    this.fps = 59.9227434 // NTSC; TODO: handle PAL
    this.interval = 1000 / this.fps
    this.paused = false

    this.basicEnabled = true
    this.keyboardJoystickEnabled = false
    this.joystick = { up: false, down: false, left: false, right: false, trigger: false }
    this.consoleKeys = { option: false, select: false, start: false }

    this.homeDir = '/'
    this.cartridge = null
    this.executable = null
    this.disk1 = null
    this.disk2 = null
    
    this.bindLib()
  }

  initialize() {
    this.lib.initDefault()
    this.startVideo()
    this.initKeyboard()
    this.initGamepad()
  }

  stop() {
    // this.stopped = true
  }

  bindLib() {
    this.lib = {
      initDefault: Module.cwrap('init_default', 'void', []),
      initWithoutBasic: Module.cwrap('init_without_basic', 'void', ['int']),
      initExecutable: Module.cwrap('init_executable', 'void', ['int']),
      warmstart: Module.cwrap('libatari800_warmstart', 'void', []),
      coldstart: Module.cwrap('libatari800_coldstart', 'void', []),
      exit: Module.cwrap('libatari800_exit', 'void', []),
      nextFrame: Module.cwrap('next_frame', 'void', []),
      getPixels: Module.cwrap('get_pixels', 'int', []),
      getAudioBuffer: Module.cwrap('get_audio_buffer', 'int', []),
      getAudioBufferLen: Module.cwrap('get_audio_buffer_len', 'int', []),
      getSoundBuffer: Module.cwrap('libatari800_get_sound_buffer', 'int', []),
      getSoundBufferLen: Module.cwrap('libatari800_get_sound_buffer_len', 'int', []),
      getSoundBufferAllocatedSize: Module.cwrap('libatari800_get_sound_buffer_allocated_size', 'int', []),
      getSoundFrequency: Module.cwrap('libatari800_get_sound_frequency', 'int', []),
      getNumSoundChannels: Module.cwrap('libatari800_get_num_sound_channels', 'int', []),
      getSoundSampleSize: Module.cwrap('libatari800_get_sound_sample_size', 'int', []),
      keyPress: Module.cwrap('key_press', 'void', ['int', 'bool', 'bool']),
      keyRelease: Module.cwrap('key_release', 'void', ['char', 'bool', 'bool']),
      consoleKeys: Module.cwrap('console_keys', 'void', ['bool', 'bool', 'bool']),
      joystick: Module.cwrap('joystick', 'void', ['int', 'char', 'char']),
      setExecutableFile: Module.cwrap('set_executable_file', 'void', ['int']),
      rebootWithFile: Module.cwrap('libatari800_reboot_with_file', 'int', []),
      setBasicEnabled: Module.cwrap('set_basic_enabled', 'void', ['bool']),
      insertCartridge: Module.cwrap('insert_cartridge', 'void', ['int']),
      removeCartridge: Module.cwrap('remove_cartridge', 'void', []),
      insertCassette: Module.cwrap('insert_cassette', 'void', ['int']),
      removeCassette: Module.cwrap('remove_cassette', 'void', []),
      mountDisk: Module.cwrap('mount_disk', 'void', ['int', 'int', 'bool']),
      unmountDisk: Module.cwrap('unmount_disk', 'void', ['int']),
    }
  }

  pause() {
    this.paused = true
  }

  resume() {
    this.paused = false
    this.drawFrame()
  }

  startAudio() {
    this.audioEnabled = true
    this.audioCtx = new AudioContext({ sampleRate: 44100 })
    this.nextSampleTime = 0
  }

  stopAudio() {
    this.audioEnabled = false
  }

  startVideo() {
    this.ctx = this.canvas.getContext('2d')
    this.lastFrame = window.performance.now()
    this.drawFrame()
  }

  advanceFrame() {
    this.lib.nextFrame()
    this.grabFrameAudio()
  }

  drawFrame() {
    if (this.paused) return

    requestAnimationFrame(() => this.drawFrame())

    const ptr = this.lib.getPixels()
    const data = new Uint8ClampedArray(Module.HEAPU8.buffer, ptr, this.width * this.height * 4);
    const img = new ImageData(data, this.width, this.height);
    this.ctx.putImageData(img, 0, 0);

    this.readGamepad()

    const now = window.performance.now()
    const elapsed = now - this.lastFrame
    if (elapsed < this.interval) return

    // for (let t = this.lastFrame; t < now; t += this.interval) {
      this.advanceFrame()
    // }

    const late = elapsed % this.interval
    this.lastFrame = now - late
  }

  grabFrameAudio() {
    if (!this.audioEnabled) return

    const ptr = this.lib.getAudioBuffer()
    const bufferLen = this.lib.getAudioBufferLen()
    if (bufferLen === 0) return

    const data = new Uint8Array(Module.HEAPU8.buffer, ptr, bufferLen)

    const buffer = this.audioCtx.createBuffer(1, bufferLen, 44100)
    // buffer.copyToChannel(0, data);
    const chanBuffer = buffer.getChannelData(0);
    for (let i = 0; i < buffer.length; i++) {
      chanBuffer[i] = (data[i] - 128) / 256;
    }

    const source = this.audioCtx.createBufferSource()
    source.buffer = buffer
    source.connect(this.audioCtx.destination)

    if (this.nextSampleTime == 0) this.nextSampleTime = this.audioCtx.currentTime
    source.start(this.nextSampleTime)
    this.nextSampleTime += buffer.length / buffer.sampleRate
  }

  onKeyDown(e) {
    e.preventDefault()
    if (this.keyboardJoystickEnabled) {
      if (this.handleKeyboardJoystickPress(e)) return
    }

    const [keyChar, keyCode] = this.getKeyCharAndCode(e)
    if (e.repeat) return

    const capsLockToggled = this.checkCapsLockToggled(e)
    if (capsLockToggled) {
      this.lib.keyPress(0, AKEY_CAPSTOGGLE, e.ctrlKey, e.shiftKey)
      requestAnimationFrame(() => this.lib.keyPress(keyChar, keyCode, e.ctrlKey, e.shiftKey))
    } else {
      this.lib.keyPress(keyChar, keyCode, e.ctrlKey, e.shiftKey)
    }
  }

  onKeyUp(e) {
    e.preventDefault()
    if (this.keyboardJoystickEnabled) {
      if (this.handleKeyboardJoystickRelease(e)) return
    }

    const [keyChar, keyCode] = this.getKeyCharAndCode(e)
    this.lib.keyRelease(keyChar, keyCode, e.ctrlKey, e.shiftKey)
  }

  getKeyCharAndCode(e) {
    if (e.key in KEY_AKEY) return [0, KEY_AKEY[e.key]]
    if (e.code in KEY_AKEY) return [0, KEY_AKEY[e.code]]
    if (e.key.length === 1) return [e.key.charCodeAt(0), 0]
    return [0, 0]
  }

  checkCapsLockToggled(e) {
    const capsLocked = e.getModifierState('CapsLock')
    if (this.capsLocked == null) this.capsLocked = capsLocked
    const toggled = capsLocked != this.capsLocked
    this.capsLocked = capsLocked
    return toggled
  }

  initKeyboard() {
    this.capsLocked = undefined

    this._onKeyDown = e => this.onKeyDown(e)
    this._onKeyUp = e => this.onKeyUp(e)

    document.addEventListener('keydown', this._onKeyDown)
    document.addEventListener('keyup', this._onKeyUp)
  }

  setKeyboardJoystickEnabled(enabled) {
    this.keyboardJoystickEnabled = enabled
  }

  handleKeyboardJoystickPress(e) {
    if (e.code === 'AltLeft') {
      this.joystick.trigger = true
    } else if (e.key === 'ArrowUp') {
      this.joystick.up = true
      this.joystick.down = false
    } else if (e.key === 'ArrowDown') {
      this.joystick.down = true
      this.joystick.up = false
    } else if (e.key === 'ArrowLeft') {
      this.joystick.left = true
      this.joystick.right = false
    } else if (e.key === 'ArrowRight') {
      this.joystick.right = true
      this.joystick.left = false
    } else {
      return false
    }
    this.updateJoystick()
    return true
  }

  handleKeyboardJoystickRelease(e) {
    if (e.code === 'AltLeft') {
      this.joystick.trigger = false
    } else if (e.key === 'ArrowUp') {
      this.joystick.up = false
    } else if (e.key === 'ArrowDown') {
      this.joystick.down = false
    } else if (e.key === 'ArrowLeft') {
      this.joystick.left = false
    } else if (e.key === 'ArrowRight') {
      this.joystick.right = false
    } else {
      return false
    }
    this.updateJoystick()
    return true
  }

  updateJoystick() {
    const { up, down, left, right, trigger } = this.joystick
    const joy = right * 8 + left * 4 + down * 2 + up
    this.lib.joystick(0, joy, trigger)
  }

  updateConsoleKeys() {
    this.lib.consoleKeys(
      this.consoleKeys.start,
      this.consoleKeys.select,
      this.consoleKeys.option,
    )
  }

  startKeyPress() {
    this.consoleKeys.start = true
    this.updateConsoleKeys()
  }

  startKeyRelease() {
    this.consoleKeys.start = false
    this.updateConsoleKeys()
  }

  optionKeyPress() {
    this.consoleKeys.start = true
    this.updateConsoleKeys()
  }

  optionKeyRelease() {
    this.consoleKeys.start = false
    this.updateConsoleKeys()
  }

  selectKeyPress() {
    this.consoleKeys.select = true
    this.updateConsoleKeys()
  }

  selectKeyRelease() {
    this.consoleKeys.select = false
    this.updateConsoleKeys()
  }

  resetKeyPress() {
    // this.lib.keyPress(0, AKEY_WARMSTART, false, false)
    this.lib.warmstart()
  }

  resetKeyRelease() {
    // this.lib.keyRelease(0, AKEY_WARMSTART, false, false)
  }

  initGamepad() {
    // TODO: Handle multiple gamepads
    window.addEventListener('gamepadconnected', e => { this.gamepad = e.gamepad })
    window.addEventListener('gamepaddisconnected', e => { this.gamepad = null })
  }

  readGamepad() {
    if (!this.gamepad) return
    const { axes, buttons } = navigator.getGamepads()[0]
    this.joystick.left = axes[0] < -0.85
    this.joystick.right = axes[0] > 0.85
    this.joystick.up = axes[1] < -0.85
    this.joystick.down = axes[1] > 0.85
    this.joystick.trigger = buttons[0].pressed || buttons[1].pressed
    this.updateJoystick()
  }

  setBasicEnabled(enabled) {
    this.basicEnabled = enabled
    this.lib.setBasicEnabled(enabled)
    if (enabled) {
      this.lib.initDefault()
    } else {
      this.lib.initWithoutBasic()
    }
  }

  createFilenamePointer(filename) {
    const filepath = `${this.homeDir}${filename}`
    const ptr = _malloc(filepath.length+1)
    stringToUTF8(filepath, ptr, filepath.length+1)
    return ptr
  }

  insertCartridge(data, filename='cartridge.car') {
    this.cartridge = { filename, data }
    this.addFile(filename, data)
    
    const ptr = this.createFilenamePointer(filename)
    this.lib.insertCartridge(ptr)
    _free(ptr)
  }

  removeCartridge() {
    this.cartridge = null
    this.lib.removeCartridge()
  }

  loadExecutable(data, filename='program.xex') {
    this.executable = { filename, data }
    this.addFile(filename, data)

    // setTimeout(() => {
      const ptr = this.createFilenamePointer(filename)
      this.lib.setExecutableFile(ptr)
      // this.lib.rebootWithFile(ptr)
      // this.lib.exit()
      this.lib.initExecutable(ptr)
      _free(ptr)
    // }, 200);
  }

  removeExecutable() {
    this.executable = null
  }

  insertDisk1(data, filename='disk1.atr') {
    this.disk1 = { filename, data }
    this.addFile(filename, data)

    const ptr = this.createFilenamePointer(filename)
    this.lib.mountDisk(1, ptr, true /* readonly */)
    _free(ptr)
  }

  ejectDisk1() {
    this.disk1 = null
    this.lib.unmountDisk(1)
  }

  insertDisk2(data, filename='disk2.atr') {
    this.disk2 = { filename, data }
    this.addFile(filename, data)


    const ptr = this.createFilenamePointer(filename)
    this.lib.mountDisk(2, ptr, true /* readonly */)
    _free(ptr)
  }

  ejectDisk2() {
    this.disk2 = null
    this.lib.unmountDisk(2)
  }

  addFile(filename, arrayBuffer) {
    const data = new Uint8Array(arrayBuffer)
    Module.FS.writeFile(`/${filename}`, data)
  }

}