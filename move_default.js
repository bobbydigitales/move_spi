

globalThis.onMidiMessageExternal = function (data) {
    console.log(`onMidiMessageExternal ${data[0].toString(16)} ${data[1].toString(16)} ${data[2].toString(16)}`);

    move_midi_internal_send([0 << 4 | (data[0] / 16), data[0], data[1], data[2]]);
}

globalThis.onMidiMessageInternal = function (data) {
    console.log(`onMidiMessageInternal ${data[0].toString(16)} ${data[1].toString(16)} ${data[2].toString(16)}`);
    move_midi_external_send([2 << 4 | (data[0] / 16), data[0], data[1], data[2]]);
}



function init() {
    console.log("Move default surface script starting...");


    // display 4ths layout

}

init();
