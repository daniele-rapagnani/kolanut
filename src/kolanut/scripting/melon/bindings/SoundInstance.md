
# Class SoundInstance



This class represents a sound that's playing or has been played.
It can be used to interact with a sound while it's playing.




## Methods

### getGain()

Gets the current gain 


**Returns:** the current gain

### getPan()

Gets the current pan 


**Returns:** the current pan: 0.0 is center, -1.0 is left, 1.0 is right

### setGain(gain)

Sets the current gain 
| Argument | Description |
| -------- | ----------- |
|  **gain**  | The gain value, 1.0 is the original sample's volume |


### setPan(pan)

Sets the current pan 
| Argument | Description |
| -------- | ----------- |
|  **pan**  | The pan value, see `getPan` |


### isPlaying()

Checks if this sound is still playing 


**Returns:** `true` if it&#39;s playing `false` otherwise

### isPaused()

Checks if this sound is paused. If a sound has been paused it is still playing as it can be resumed. 


**Returns:** `true` if it&#39;s paused `false` otherwise

### setPaused(paused)

Pause or resume this sound 
| Argument | Description |
| -------- | ----------- |
|  **paused**  | `true` for pausing, `false` for resuming |


### isLooping()

Checks if this sound's looping is enabled. 


**Returns:** `true` if it&#39;s looping `false` otherwise

### setLooping(paused)

Enable/disables looping for this sound 
| Argument | Description |
| -------- | ----------- |
|  **paused**  | `true` for looping, `false` otherwise |


### stop()

Stop the sound. Once stopped the sound's instance will be released and you will not be able to play this sound anymore.


### restart()

Restarts playing this sound from the beginning






