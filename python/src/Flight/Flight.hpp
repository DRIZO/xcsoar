/* Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
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

#ifndef PYTHON_FLIGHT_HPP
#define PYTHON_FLIGHT_HPP

#include "IGCFixEnhanced.hpp"
#include "DebugReplayIGC.hpp"
#include "DebugReplayVector.hpp"
#include "FlightTimes.hpp"
#include "AnalyseFlight.hpp"

#include <vector>

class DebugReplay;

class Flight {
private:
  std::vector<IGCFixEnhanced> *fixes;
  bool keep_flight;
  const char *flight_file;

public:
  /**
   * Create a empty flight object, used to create a flight from in-memory data
   */
  Flight()
    : keep_flight(true), flight_file(nullptr) {
    fixes = new std::vector<IGCFixEnhanced>;
  };

  /**
   * Destructor
   */
  ~Flight();

  /**
   * Create a flight object with file source
   */
  Flight(const char* _flight_file, bool _keep_flight);

  /**
   * Return a DebugReplay, either direct from file or from memory,
   * depending on the keep_flight flag. Don't forget to delete
   * the replay after use.
   */
  DebugReplay *Replay() {
    if (keep_flight) return DebugReplayVector::Create(*fixes);
    else return DebugReplayIGC::Create(flight_file);
  };

  /* Search for flights within the fixes */
  unsigned Times(std::vector<FlightTimeResult> &results) {
    DebugReplay *replay = Replay();
    FlightTimes(*replay, results);
    delete replay;

    return results.size();
  };

  /**
   * Calculate the DP reduced flight path
   * This always sets the keep_flight flag to true and
   * stores the flight fixes in memory
   */
  void Reduce(const BrokenDateTime start, const BrokenDateTime end,
              const unsigned num_levels, const unsigned zoom_factor,
              const double threshold, const bool force_endpoints,
              const unsigned max_delta_time, const unsigned max_points);

  /* Analyse flight */
  void Analyse(const BrokenDateTime takeoff_time,
               const BrokenDateTime release_time,
               const BrokenDateTime landing_time,
               ContestStatistics &olc_plus,
               ContestStatistics &dmst,
               PhaseList &phase_list,
               PhaseTotals &phase_totals,
               WindList &wind_list,
               const unsigned full = 512,
               const unsigned triangle = 1024,
               const unsigned sprint = 96,
               const unsigned max_iterations = 20e6,
               const unsigned max_tree_size = 5e6) {
    DebugReplay *replay = Replay();
    AnalyseFlight(*replay, takeoff_time, release_time, landing_time,
                  olc_plus, dmst,
                  phase_list, phase_totals, wind_list,
                  full, triangle, sprint,
                  max_iterations, max_tree_size);
    delete replay;
  };

  /**
   * Append a fix to this flight (only valid for in-memory flights)
   */
  void AppendFix(const IGCFixEnhanced &fix) {
    if (fixes == nullptr) return;

    fixes->push_back(fix);
  };

private:
  /* Read the flight into memory */
  void ReadFlight();
};

#endif /* PYTHON_FLIGHT_HPP */
