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

#include "FAISectorRenderer.hpp"
#include "Engine/Task/Shapes/FAITriangleSector.hpp"
#include "Geo/GeoPoint.hpp"
#include "Projection/Projection.hpp"
#include "Screen/Canvas.hpp"

void
RenderFAISector(Canvas &canvas, const Projection &projection,
                const GeoPoint &pt1, const GeoPoint &pt2,
                bool reverse)
{
  GeoPoint geo_points[FAI_TRIANGLE_SECTOR_MAX];
  GeoPoint *geo_end = GenerateFAITriangleSector(geo_points, pt1, pt2, reverse);

  RasterPoint points[FAI_TRIANGLE_SECTOR_MAX], *p = points;
  for (GeoPoint *geo_i = geo_points; geo_i != geo_end;)
    *p++ = projection.GeoToScreen(*geo_i++);

  canvas.DrawPolygon(points, p - points);
}