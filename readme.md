# Security for Internet of Things
##Authentication and Authorization for Bluetooth LE Devices – Over The Air Download Firmware Update



## Synopsis
Current IoT(Internet of Things) implementations do not have a authentication/authorization feature for their Android based `Over The Air` Firmware update. I took on the challenge
of creating one from scratch. By using some neat little encryption techniques, I developed an algorithm (look below) that requires client(android) authentication and authorization before it can go ahead with the OAD update.

You can find the report and other links. All source code is also mentioned in the project. 

I have also included a Powerpoint Presentation for the folks who don't want to go through the report, although I would highly recommend it.

## Code Example
The examples are mentioned in the report. Look at 'em. 
If you have trouble understanding it, email me at : [shreyas.enug@gmail.com](shreyas.enug@gmail.com)


## Motivation
Think about this. You have a reminder sensor telling you what time you should take your medecine, a temperature controller at home, some automated device speaking to your phone, a pacemaker.
Now picture me coming along and making my own malicious version of a firmware and uploading it to that device to misbehave. How would that make you feel? Not very good. This is an important issue.
I just found a way to take care of those pesky crackers and wrongdoers by putting a low-overhead algorithm implementation that only allows authorized parties to put the firmware into the IoT device.
There is no known implementation out there; this is the first one, but I'm sure there are more to come. 



## Installation
It's pretty straight forward actually. Just put the `.apk` into your android phone and install. 
Now, to put the primary BLE hex files onto the SensorTag, you're gonna have to flash them onto your device using TI Code Composer and BLE stack found [here](http://www.ti.com/tool/sensortag-sw)
Then switch the sensortag on, detect it on your android phone using the `apk` and follow the video tutorial.

NOTE : ##The actual Firmware Update service is not implemented in the current build, but will be added later.


## API Reference
Mostly Google APIs : Fonts , Geolocation and Mapping. They've made things really easy. The more I do these projects, the more I learn how nifty their APIs are.

##Video : Project in Action
[![GuneSec IoT OAD Authentication/Authorization](image src)](https://youtu.be/GaJanlhgQm0 "GuneSec IoT Security")

## Tests
No test cases were written. I manually tested it relentlessly but I never wrote automated tests. I know I should've have, but I just wanted to hack it enough to make it work.

## Contributors
If you want to contribute or add to it or make it better, more readable, go for it. Tweet me issues if you can  : [@shreyaslumos](https://www.twitter.com/shreyaslumos) 

## License

<a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-nd/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">GuneSec IoT Security for BLE OAD</span> by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Shreyas Gune</span> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-nd/4.0/">Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License</a>.

