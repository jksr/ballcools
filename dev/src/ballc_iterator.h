#ifndef _BALLC_ITERATOR_H_
#define _BALLC_ITERATOR_H_

#include "ballc_files.h"
#include "ballc.h"
#include "ballc_index_core.h"

class MCRecordIterator {
public:
    MCRecordIterator(const BAllC& ballc, IndexVec::const_iterator start_iter,
                       IndexVec::const_iterator end_iter, 
                       int ref_id, int start, int end);

    void AdavanceToNextValid();

    bool HasNext() const ;

    MCRecord2 Next();
    // std::string NextLine();
    bool RefIdMatch(int id1, int id2);


private:
    MCRecord Next_();

    BAllC ballc;
    IndexVec::const_iterator curr_iter;
    IndexVec::const_iterator end_iter;
    int ref_id, start, end;
    MCRecord next_rec;
    bool reach_end;
    bool inited;

};


#endif//_BALLC_ITERATOR_H_