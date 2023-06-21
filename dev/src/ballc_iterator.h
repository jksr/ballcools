#ifndef _BALLC_ITERATOR_H_
#define _BALLC_ITERATOR_H_

#include <vector>
// #include <htslib/bgzf.h>
#include "ballc_files.h"
#include "ballc_index_core.h"
#include "ballc.h"

class MCRecordIterator {
    BAllc ballc;
    IndexVec::const_iterator curr_iter;
    IndexVec::const_iterator end_iter;
    int ref_id, start, end;
    MCRecord next_rec;
    bool reach_end;
    bool inited;
    //next: return next_rec, adavance pointer, preload next_rec
    //
    
public:
    MCRecordIterator(const BAllc& ballc, IndexVec::const_iterator start_iter,
                       IndexVec::const_iterator end_iter, 
                       int ref_id, int start, int end)
        : ballc(ballc), curr_iter(start_iter), end_iter(end_iter), ref_id(ref_id), start(start), end(end), reach_end(false), inited(false){
        // Seek to the first record in the range
        AdavanceToNextValid();
    }

    void AdavanceToNextValid() {
        if(reach_end){
            return;
        }

        bool found = false;
        for( ; this->curr_iter != this->end_iter; ++(this->curr_iter)){
            if(this->curr_iter->key.ref_id!=ref_id){
                continue;
            }
            if(!inited){
                if (this->ballc.Seek(this->curr_iter->chunk_start) < 0) {
                    throw std::runtime_error("Error seeking to position in file");
                }
            }
            while (this->ballc.Tell() <= this->curr_iter->chunk_end && ballc.ReadMcRecord(this->next_rec)) {
                if(this->next_rec.ref_id == ref_id && this->next_rec.pos >= start && this->next_rec.pos < end){
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

    bool HasNext() const {
        return reach_end==false;
    }

    MCRecord Next() {
        if (!HasNext()) {
            throw std::runtime_error("No more records");
        }
        MCRecord record(this->next_rec);
        AdavanceToNextValid();
        return record;
    }
};


#endif//_BALLC_ITERATOR_H_