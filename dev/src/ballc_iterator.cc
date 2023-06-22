#include "ballc_iterator.h"


MCRecordIterator::MCRecordIterator(const BAllC& ballc, IndexVec::const_iterator start_iter,
                                    IndexVec::const_iterator end_iter, int ref_id, int start, int end
                                  ) : ballc(ballc), curr_iter(start_iter), end_iter(end_iter), ref_id(ref_id), 
                                      start(start), end(end), reach_end(false), inited(false){
    // Seek to the first record in the range
    AdavanceToNextValid();
}

void MCRecordIterator::AdavanceToNextValid() {
    if(reach_end){
        return;
    }

    bool found = false;
    for( ; this->curr_iter != this->end_iter; ++(this->curr_iter)){

        if(!RefIdMatch(this->curr_iter->key.ref_id, ref_id)){
            continue;
        }
        if(!inited){
            if (this->ballc.Seek(this->curr_iter->chunk_start) < 0) {
                throw std::runtime_error("Error seeking to position in file");
            }
        }
        while (this->ballc.Tell() <= this->curr_iter->chunk_end && ballc.ReadMcRecord(this->next_rec)) {
            if(RefIdMatch(this->curr_iter->key.ref_id, ref_id) && this->next_rec.pos >= start && this->next_rec.pos < end){
                found = true;
                break;
            }
        }
        if(found){
            break;
        }
    }
    if(!inited){
        inited = true;
    }
    if(!found){
        reach_end = true;
    }
}

bool MCRecordIterator::HasNext() const {
    return reach_end==false;
}

bool MCRecordIterator::RefIdMatch(int id1, int id2){
    return (id1==-1||id2==-1) || (id1==id2);
}

MCRecord MCRecordIterator::Next_() {
    if (!HasNext()) {
        throw std::runtime_error("No more records");
    }
    MCRecord record(this->next_rec);
    AdavanceToNextValid();
    return record;
}

MCRecord2 MCRecordIterator::Next() {
    return this->ballc.McRecordToMcRecord2(Next_());
}

