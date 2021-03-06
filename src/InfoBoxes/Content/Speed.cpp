/*
Copyright_License {

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

#include "InfoBoxes/Content/Speed.hpp"
#include "InfoBoxes/Data.hpp"
#include "Interface.hpp"

#include "Simulator.hpp"
#include "Blackboard/DeviceBlackboard.hpp"
#include "Components.hpp"
#include "Language/Language.hpp"
#include "Units/Units.hpp"

void
InfoBoxContentSpeedGround::Update(InfoBoxData &data)
{
  const NMEAInfo &basic = CommonInterface::Basic();
  if (!basic.ground_speed_available) {
    data.SetInvalid();
    return;
  }

  data.SetValueFromSpeed(basic.ground_speed);
}

bool
InfoBoxContentSpeedGround::HandleKey(const InfoBoxKeyCodes keycode)
{
  if (!is_simulator())
    return false;
  if (!CommonInterface::Basic().gps.simulator)
    return false;

  fixed fixed_step = (fixed)Units::ToSysSpeed(fixed(10));
  const Angle a5 = Angle::Degrees(5);

  switch (keycode) {
  case ibkUp:
    device_blackboard->SetSpeed(
        CommonInterface::Basic().ground_speed + fixed_step);
    return true;

  case ibkDown:
    device_blackboard->SetSpeed(std::max(fixed(0), CommonInterface::Basic().ground_speed - fixed_step));
    return true;

  case ibkLeft:
    device_blackboard->SetTrack(CommonInterface::Basic().track - a5);
    return true;

  case ibkRight:
    device_blackboard->SetTrack(CommonInterface::Basic().track + a5);
    return true;
  }

  return false;
}

void
UpdateInfoBoxSpeedIndicated(InfoBoxData &data)
{
  const NMEAInfo &basic = CommonInterface::Basic();
  if (!basic.airspeed_available) {
    data.SetInvalid();
    return;
  }

  data.SetValueFromSpeed(basic.indicated_airspeed, false);
}

void
UpdateInfoBoxSpeed(InfoBoxData &data)
{
  const NMEAInfo &basic = CommonInterface::Basic();
  if (!basic.airspeed_available) {
    data.SetInvalid();
    return;
  }

  data.SetValueFromSpeed(basic.true_airspeed, false);
}

void
UpdateInfoBoxSpeedMacCready(InfoBoxData &data)
{
  const CommonStats &common_stats = CommonInterface::Calculated().common_stats;
  data.SetValueFromSpeed(common_stats.V_block, false);
}

void
UpdateInfoBoxSpeedDolphin(InfoBoxData &data)
{
  // Set Value
  const DerivedInfo &calculated = CommonInterface::Calculated();
  data.SetValueFromSpeed(calculated.V_stf, false);

  // Set Comment
  if (CommonInterface::GetComputerSettings().features.block_stf_enabled)
    data.SetComment(_("BLOCK"));
  else
    data.SetComment(_("DOLPHIN"));

}
