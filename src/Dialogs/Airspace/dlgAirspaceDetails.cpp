/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
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

#include "Airspace.hpp"
#include "Dialogs/WidgetDialog.hpp"
#include "Form/RowFormWidget.hpp"
#include "Form/ActionListener.hpp"
#include "Airspace/AbstractAirspace.hpp"
#include "Airspace/ProtectedAirspaceWarningManager.hpp"
#include "Formatter/UserUnits.hpp"
#include "Formatter/AirspaceFormatter.hpp"
#include "Units/Units.hpp"
#include "UIGlobals.hpp"
#include "Interface.hpp"
#include "Language/Language.hpp"
#include "Compiler.h"

#include <assert.h>

class AirspaceDetailsWidget : public RowFormWidget, public ActionListener {
  const AbstractAirspace &airspace;
  ProtectedAirspaceWarningManager *warnings;

public:
  /**
   * Hack to allow the widget to close its surrounding dialog.
   */
  ActionListener *listener;

  AirspaceDetailsWidget(const AbstractAirspace &_airspace,
                        ProtectedAirspaceWarningManager *_warnings)
    :RowFormWidget(UIGlobals::GetDialogLook()),
     airspace(_airspace), warnings(_warnings) {}

  /* virtual methods from class Widget */
  virtual void Prepare(ContainerWindow &parent,
                       const PixelRect &rc) gcc_override;

  /* methods from ActionListener */
  virtual void OnAction(int id) gcc_override;
};

void
AirspaceDetailsWidget::Prepare(ContainerWindow &parent, const PixelRect &rc)
{
  const NMEAInfo &basic = CommonInterface::Basic();
  TCHAR buffer[64];

  AddMultiLine(airspace.GetName());

  if (!airspace.GetRadioText().empty())
    AddReadOnly(_("Radio"), nullptr, airspace.GetRadioText().c_str());

  AddReadOnly(_("Type"), nullptr, AirspaceFormatter::GetClass(airspace));

  AirspaceFormatter::FormatAltitude(buffer, airspace.GetTop());
  AddReadOnly(_("Top"), nullptr, buffer);

  AirspaceFormatter::FormatAltitude(buffer, airspace.GetBase());
  AddReadOnly(_("Base"), nullptr, buffer);

  if (warnings != nullptr) {
    const GeoPoint closest =
      airspace.ClosestPoint(basic.location, warnings->GetProjection());
    const fixed distance = closest.Distance(basic.location);

    FormatUserDistance(distance, buffer);
    AddReadOnly(_("Range"), nullptr, buffer);
  }
}

void
AirspaceDetailsWidget::OnAction(int id)
{
  assert(warnings != nullptr);

  bool acked = warnings->get_ack_day(airspace);
  warnings->acknowledge_day(airspace, !acked);

  listener->OnAction(mrOK);
}

void
dlgAirspaceDetails(const AbstractAirspace &airspace,
                   ProtectedAirspaceWarningManager *warnings)
{
  AirspaceDetailsWidget *widget =
    new AirspaceDetailsWidget(airspace, warnings);
  WidgetDialog dialog(_("Airspace Details"), widget);
  dialog.AddButton(_("Close"), mrOK);

  if (warnings != nullptr) {
    widget->listener = &dialog;
    dialog.AddButton(warnings->get_ack_day(airspace)
                     ? _("Enable") : _("Ack Day"),
                     widget, 1);
  }

  dialog.ShowModal();
}