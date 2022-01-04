
//var model;

WorkerScript.onMessage = function(message) {

//    if (message.model != undefined) {
//        console.log("model" + message.model)
//        model = message.model;
//        return;
//    }

//    console.log("worker " + message.item);

//    for(var n in message) {
//        console.log("    " + n + ":" + message[n])
//    }

//    console.log("index" + model[message.index])

    WorkerScript.sendMessage( { item: message.item } );
}

