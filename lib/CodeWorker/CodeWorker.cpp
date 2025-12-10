#include "CodeWorker.h"

void CodeWorker::setup(){

}

void CodeWorker::update(){
    if(decoder == nullptr) return;
    if(rtdb == nullptr) return;

    if(decoder->getSequence().length() < 8) return;
    if(decoder->getSequence()[7] != '#') return;
    if(decoder->getSequence()[0] != '*') return;

    rtdb->pushString("/codes", decoder->getSequence());
    decoder->clearSequence();
}