//
//  scrntime.h
//  uClock
//
//  Created by uRyn on 2020/1/24.
//  Copyright © 2020 uRyn. All rights reserved.
//

#ifndef _URYN_SCRNTIME_H_
#define _URYN_SCRNTIME_H_

class scrntime
{
public:
    void update_time();
    bool _isnewyear = false;
    float _nhourdeg;
    float _nmindeg;
    float _nsecdeg;
};

#endif /* _URYN_SCRNTIME_H_ */
