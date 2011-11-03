/* Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "harness_flight.hpp"
#include "harness_airspace.hpp"
#include "harness_waypoints.hpp"
#include "TaskEventsPrint.hpp"
#include <fstream>

extern double time_elapsed;
extern double calc_cruise_efficiency;
extern double calc_effective_mc;


bool test_speed_factor(int test_num, int n_wind) 
{
  // flying at opt speed should be minimum time flight!

  double te0, te1, te2;

  TestFlightResult result = test_flight(test_num, n_wind, 1.0);
  te0 = result.time_elapsed;

  result = test_flight(test_num, n_wind, 0.7);
  te1 = result.time_elapsed;
  // time of this should be higher than nominal
  ok(te0<te1, test_name("vopt slow or",test_num, n_wind), 0);

  result = test_flight(test_num, n_wind, 1.5);
  te2 = result.time_elapsed;
  // time of this should be higher than nominal
  ok(te0<te2, test_name("vopt fast or",test_num, n_wind), 0);

  bool retval = (te0<te1) && (te0<te2);
  if (verbose || !retval) {
    printf("# sf 0.8 time_elapsed_rat %g\n",te1/te0);
    printf("# sf 1.2 time_elapsed_rat %g\n",te2/te0);
  }
  return retval;
}

bool test_abort(int n_wind)
{
  GlidePolar glide_polar(fixed_two);
  Waypoints waypoints;
  setup_waypoints(waypoints);

  if (verbose) {
    distance_counts();
  }

  TaskEventsPrint default_events(verbose);

  TaskManager task_manager(default_events,
                           waypoints);

  TaskBehaviour task_behaviour = task_manager.GetTaskBehaviour();
  task_behaviour.DisableAll();
  task_behaviour.enable_trace = false;
  task_manager.SetTaskBehaviour(task_behaviour);

  task_manager.SetGlidePolar(glide_polar);

  test_task(task_manager, waypoints, 1);

  task_manager.Abort();
  task_report(task_manager, "abort");

  autopilot_parms.goto_target = true;
  return run_flight(task_manager, autopilot_parms, n_wind);
}


bool test_goto(int n_wind, unsigned id, bool auto_mc)
{
  GlidePolar glide_polar(fixed_two);
  Waypoints waypoints;
  setup_waypoints(waypoints);

  if (verbose) {
    distance_counts();
  }

  TaskEventsPrint default_events(verbose);

  TaskManager task_manager(default_events,
                           waypoints);


  TaskBehaviour task_behaviour = task_manager.GetTaskBehaviour();
  task_behaviour.DisableAll();
  task_behaviour.auto_mc = auto_mc;
  task_behaviour.enable_trace = false;
  task_manager.SetTaskBehaviour(task_behaviour);

  task_manager.SetGlidePolar(glide_polar);

  test_task(task_manager, waypoints, 1);

  task_manager.DoGoto(*waypoints.lookup_id(id));
  task_report(task_manager, "goto");

  waypoints.clear(); // clear waypoints so abort wont do anything

  autopilot_parms.goto_target = true;
  return run_flight(task_manager, autopilot_parms, n_wind);
}


bool test_null()
{
  GlidePolar glide_polar(fixed_two);
  Waypoints waypoints;
  setup_waypoints(waypoints);

  if (verbose) {
    distance_counts();
  }

  TaskEventsPrint default_events(verbose);

  TaskManager task_manager(default_events,
                           waypoints);

  TaskBehaviour task_behaviour = task_manager.GetTaskBehaviour();
  task_behaviour.DisableAll();
  task_behaviour.enable_trace = false;
  task_manager.SetTaskBehaviour(task_behaviour);

  task_manager.SetGlidePolar(glide_polar);

  task_report(task_manager, "null");

  waypoints.clear(); // clear waypoints so abort wont do anything

  autopilot_parms.goto_target = true;
  return run_flight(task_manager, autopilot_parms, 0);
}

bool test_olc(int n_wind, Contests olc_type)
{
  GlidePolar glide_polar(fixed_two);
  Waypoints waypoints;
  setup_waypoints(waypoints);

  if (verbose) {
    distance_counts();
  }

  TaskEventsPrint default_events(verbose);

  TaskManager task_manager(default_events,
                           waypoints);

  TaskBehaviour task_behaviour = task_manager.GetTaskBehaviour();
  task_behaviour.DisableAll();
  task_behaviour.enable_olc = true;
  if (!verbose)
    task_behaviour.enable_trace = false;
  task_manager.SetTaskBehaviour(task_behaviour);

  task_manager.SetGlidePolar(glide_polar);
  test_task(task_manager, waypoints, 1);

  waypoints.clear(); // clear waypoints so abort wont do anything

  autopilot_parms.goto_target = true;
  return run_flight(task_manager, autopilot_parms, n_wind);
}
