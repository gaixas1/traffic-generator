/*
 * Traffic generator
 *
 *   Addy Bombeke <addy.bombeke@ugent.be>
 *   Dimitri Staessens <dimitri.staessens@intec.ugent.be>
 *   Douwe De Bock <douwe.debock@ugent.be>
 *   Sander Vrijders <sander.vrijders@intec.ugent.be>
 *
 * This source code has been released under the GEANT outward license.
 * Refer to the accompanying LICENSE file for further information
 */

#pragma once

#include <librina/librina.h>
#include <time.h>
#include <signal.h>

#include "simple_ap.h"

class simple_server: public simple_ap
{
public:
	void run();

protected:
	virtual void handle_flow(int port_id, int fd) = 0;
};
