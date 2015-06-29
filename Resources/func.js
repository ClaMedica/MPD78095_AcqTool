function fromDoubleTimeToString(time)
{
    var tim=time
    var ore=Math.floor(tim/3600000)
    tim=tim-ore*3600000
    var min=Math.floor(tim/60000)
    tim=tim-min*60000
    var sec=Math.floor(tim/1000)
    tim=tim-sec*1000
    var msec=tim
    if(min<10)
        min='0'+min
    if(sec<10)
        sec='0'+sec
    msec=msec.toFixed(0)
    if(msec<10)
        msec='00'+msec
    else if(msec<100)
        msec='0'+msec
    return ore + ':' + min + ':' + sec + ':' + msec
}

function newId(){
    return Math.round(Math.random()*10000)
}

function decDigits(val)
{
    if(val>=1)       return 0
    if(val>=0.1)     return 1
    if(val>=0.01)    return 2
    if(val>=0.001)   return 3
    if(val>=0.0001)  return 4
    if(val>=0.00001) return 5
}
