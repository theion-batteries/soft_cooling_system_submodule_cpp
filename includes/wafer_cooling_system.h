/**
 * @file wafer_cooling_system.h
 * @author sami dhiab (sami@theion.de)
 * @brief wafer cooling system package
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <iostream>
#pragma once

namespace wafer_cooling_system
{
    /****** cooling rotation *******/
    // interface
    class Icooling_rotation
    {
    public:
        Icooling_rotation();
        virtual ~Icooling_rotation();
        virtual void ph_rotate() = 0;
    };
    // implementation  
    class cooling_rotation: public Icooling_rotation
    {
        protected:
        virtual void ph_rotate();
    };

    /******* cooling motion *****/
    // interface
    class Icooling_motion
    {
    public:
        Icooling_motion();
        virtual ~Icooling_motion();
        virtual void move_down_to_center() = 0;
        virtual void move_up_to_reference() = 0;
    };
    // implementation 
    class cooling_motion: public Icooling_motion
    {
        virtual void move_down_to_center();
        virtual void move_up_to_reference();
    };

    /****** cooling printing ********/
    // interface

    class Icooling_spitting
    {
    public:
    Icooling_spitting();
    virtual ~Icooling_spitting();
        virtual void ph_spit() = 0;
    };
    // implementation 
    class cooling_spitting: public Icooling_spitting
    {
        virtual void ph_spit();
    };

    /****** cooling controller ********/
    // interface
    class Icooling_controller
    {
    public:
    Icooling_controller();
    virtual ~Icooling_controller();
        virtual void start_cooling() = 0;
        virtual void stop_cooling() = 0;
    };
    // implementation 
    class cooling_controller: public Icooling_controller
    {
        private:
        Icooling_motion* ph_motion;
        Icooling_rotation* ph_rotation;
        Icooling_spitting* ph_printing;
        protected:
        virtual void start_cooling();
        virtual void stop_cooling();
        public:
        cooling_controller() ;
        virtual ~cooling_controller();

    };


}

