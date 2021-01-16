//  scanner.cpp -- API implementation for an ESC/I driver
//  Copyright (C) 2012-2015  SEIKO EPSON CORPORATION
//  Copyright (C) 2013  Olaf Meeuwissen
//
//  License: GPL-3.0+
//  Author : EPSON AVASYS CORPORATION
//
//  This file is part of the 'Utsushi' package.
//  This package is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License or, at
//  your option, any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//  You ought to have received a copy of the GNU General Public License
//  along with this package.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <utsushi/i18n.hpp>
#include <utsushi/log.hpp>

#include "compound-scanner.hpp"
#include "compound-tweaks.hpp"
#include "extended-scanner.hpp"
#include "extended-tweaks.hpp"
#include "getter.hpp"
#include "scanner.hpp"
#include "scanner-inquiry.hpp"
#include "get-extended-status.hpp"

#ifdef HAVE_STANDARD_SCANNER
#include "standard-scanner.hpp"
#endif

namespace utsushi {

extern "C" {

bool
is_interpreter (const std::string& cnx)
{
  return !(cnx == "usb" || cnx == "networkscan");
}

void
libdrv_esci_LTX_scanner_factory (const scanner::info& info, scanner::ptr& rv)
{
  std::string cnx_type = info.connexion ();
  std::string cnx_path = info.path ();
  bool cnx_debug = info.enable_debug ();

  if (is_interpreter (cnx_type))        // expand shorthand notation
    {
      cnx_type.insert (0, "esci-");
      if (0 != cnx_path.find ("usb:"))
        cnx_path.insert (0, "usb:");
    }
  connexion::ptr cnx (connexion::create (cnx_type, cnx_path, cnx_debug));

  using namespace _drv_;

  esci::scanner::ptr sp;

  using namespace esci;

  if (!cnx)
    {
      log::fatal
        ("expected an established connexion");

      return;
    }

  try
    {
      scanner_inquiry FS_Y;
      information info;

      *cnx << FS_Y.get (info);
      *cnx << FS_Y.finish ();

      log::brief ("detected a '%1%'") % info.product_name ();

      /**/ if (info.product_name () == "DS-40")
        {
          sp = make_shared< DS_40 > (cnx);
        }
      else if (   info.product_name () == "DS-310"
               || info.product_name () == "DS-320"
               || info.product_name () == "DS-360W"
               || info.product_name () == "ES-200"
               || info.product_name () == "ES-300W"
               || info.product_name () == "ES-300WR"
               )
        {
          sp = make_shared< DS_3x0 > (cnx);
        }
      else if (info.product_name () == "DS-410")
        {
          sp = make_shared< DS_4x0 > (cnx);
        }
      else if (   info.product_name () == "DS-510"
               || info.product_name () == "DS-520"
               || info.product_name () == "DS-560"
               )
        {
          sp = make_shared< DS_5x0 > (cnx);
        }
      else if (   info.product_name () == "DS-760"
               || info.product_name () == "DS-860"
               || info.product_name () == "FF-640"
               )
        {
          sp = make_shared< DS_760_860 > (cnx);
        }
      else if (   info.product_name () == "DS-5500"
               || info.product_name () == "DS-6500"
               || info.product_name () == "DS-7500"
              )
        {
          sp = make_shared< DS_x500 > (cnx);
        }
      else if (   info.product_name () == "DS-50000"
               || info.product_name () == "DS-60000"
               || info.product_name () == "DS-70000"
              )
        {
          sp = make_shared< DS_x0000 > (cnx);
        }
      else if (   info.product_name () == "DS-530"
               || info.product_name () == "DS-570W"
               || info.product_name () == "DS-770"
               || info.product_name () == "DS-780N"
               || info.product_name () == "ES-400"
               || info.product_name () == "ES-500W"
               || info.product_name () == "DS-535"
               || info.product_name () == "DS-535H"
               || info.product_name () == "DS-775"
               || info.product_name () == "DS-575W"
               || info.product_name () == "FF-680W"
               || info.product_name () == "ES-500WR"
               )
        {
          sp = make_shared< DS_530_570W > (cnx);
        }
      else if (   info.product_name () == "DS-1610"
               || info.product_name () == "DS-1630"
               || info.product_name () == "DS-1660W"
               )
        {
          sp = make_shared< DS_16x0 > (cnx);
        }
      else if (   info.product_name () == "PID 08BC"
               || info.product_name () == "PID 08CC"
               || info.product_name () == "PID 08CE"
               || info.product_name () == "PID 08CF"
               || info.product_name () == "PID 1112"
               || info.product_name () == "PID 1125"
               || info.product_name () == "PID 1126"
               || info.product_name () == "PID 1127"
               || info.product_name () == "PID 1128"
	             || info.product_name () == "PID 112D"
	             || info.product_name () == "PID 113B"
	             || info.product_name () == "PID 113C"
               || info.product_name () == "PID 1152"
	             || info.product_name () == "PID 1153"
               )
        {
          sp = make_shared< PX_Mxxxx > (cnx);
        }
      else if (info.product_name () == "PID 111B"
	    || info.product_name () == "PID 1131"
	    || info.product_name () == "PID 1132"
      || info.product_name () == "PID 114B"
      || info.product_name () == "PID 114C"
	    )
        {
          sp = make_shared< EP_8xxA > (cnx);
        }
      else if (info.product_name () == "PID 1123"
	    )
        {
          sp = make_shared< EP_30VA > (cnx);
        }
      else if (info.product_name () == "PID 112E"
	    || info.product_name () == "PID 112F"
	    || info.product_name () == "PID 1140"
      || info.product_name () == "PID 1159"
	    )
        {
          sp = make_shared< PX_M884F > (cnx);
        }
      else if (info.product_name () == "PID 1133"
	    )
        {
          sp = make_shared< EP_810A > (cnx);
        }
      else if (info.product_name () == "PID 1134"
      || info.product_name () == "PID 1146"
	    )
        {
          sp = make_shared< EP_71xA > (cnx);
        }
      else if (info.product_name () == "PID 1129"
	    || info.product_name () == "PID 112A"
            || info.product_name () == "PID 1130"
            || info.product_name () == "PID 1135"
	          || info.product_name () == "PID 1139"
	          || info.product_name () == "PID 113A"
            || info.product_name () == "PID 114A"
            || info.product_name () == "PID 1138"
	          || info.product_name () == "PID 1136"
            || info.product_name () == "PID 1137"
            || info.product_name () == "PID 1154"
            || info.product_name () == "PID 1155"
            || info.product_name () == "PID 1156"
            || info.product_name () == "PID 1164"
            || info.product_name () == "PID 1165"
            || info.product_name () == "PID 1166"
	    )
        {
          sp = make_shared< L61x0 > (cnx);
        }
      else if (   info.product_name () == "PID 112B"
	       || info.product_name () == "PID 112C"
              )
        {
          sp = make_shared< ET_77xx > (cnx);
        }
      else if (   info.product_name () == "PID 1143"
               || info.product_name () == "PID 1157"
               || info.product_name () == "PID 1158"
              )
        {
          sp = make_shared< L3xxx > (cnx);
        }
      else if (   info.product_name () == "PID 1147"
              )
        {
          sp = make_shared< XP_7xxx > (cnx);
        }
      else if (   info.product_name () == "PID 1145"
              )
        {
          sp = make_shared< EP_81xx > (cnx);
        }
      else if (   info.product_name () == "PID 1148"
              )
        {
          sp = make_shared< PX_M8xxFX > (cnx);
        }
      else if (   info.product_name () == "PID 114D"
              || info.product_name () == "PID 114F"
              || info.product_name () == "PID 1150"
              || info.product_name () == "PID 1151"
              || info.product_name () == "PID 1160"
	            )
        {
          sp = make_shared< L5xxx > (cnx);
        }
      else if (   info.product_name () == "PID 1149"
              )
        {
          sp = make_shared< WF_37xx > (cnx);
        }
      else if (  info.product_name () == "ES-50"
              || info.product_name () == "ES-60W"
              || info.product_name () == "DS-70"
              || info.product_name () == "DS-80W"
              || info.product_name () == "ES-55R"
              || info.product_name () == "ES-65WR"
              || info.product_name () == "RR-60"
              || info.product_name () == "RR-70W"
              )
        {
          sp = make_shared< ES_50 > (cnx);
        }
      else if (info.product_name () == "PID 114E"
	          )
        {
          sp = make_shared< XP21xx > (cnx);
        }
      else if (info.product_name () == "PID 1161"
            || info.product_name () == "PID 116B"
	          )
        {
          sp = make_shared< LX_10xxx > (cnx);
        }
      else if (info.product_name () == "PID 115A"
	          )
        {
          sp = make_shared< EP8xxA > (cnx);
        }
      else if (info.product_name () == "PID 115B"
            || info.product_name () == "PID 115D"
	          )
        {
          sp = make_shared< EP9xxA3 > (cnx);
        }
      else if (info.product_name () == "PID 115C"
	          )
        {
          sp = make_shared< EP7xxA > (cnx);
        }
      else if (info.product_name () == "PID 1163"
	          )
        {
          sp = make_shared< EPM5xxT > (cnx);
        }
      else if (info.product_name () == "PID 1162"
            || info.product_name () == "PID 1169"
            || info.product_name () == "PID 116A"
            || info.product_name () == "PID 116E"
	          )
        {
          sp = make_shared< EWM7xxTR > (cnx);
        }
      else if (info.product_name () == "PID 1167"
            || info.product_name () == "PID 1168"
            || info.product_name () == "PID 116F"
            || info.product_name () == "PID 1170"
            || info.product_name () == "PID 1177"
	          )
        {
          sp = make_shared< ET_16xxx > (cnx);
        }
      else if (info.product_name () == "PID 116C"
            || info.product_name () == "PID 116D"
	          )
        {
          sp = make_shared< WFC8xxR > (cnx);
        }
      else if (info.product_name () == "PID 1174"
            || info.product_name () == "PID 1175"
            || info.product_name () == "PID 1176"
	          )
        {
          sp = make_shared< ET_58xx > (cnx);
        }
      else if (info.product_name () == "PID 1171"
            || info.product_name () == "PID 1172"
            || info.product_name () == "PID 1173"
	          )
        {
          sp = make_shared< WFC2xxxx > (cnx);
        }
      else if (info.product_name () == "PID 117A"
            || info.product_name () == "PID 117B"
            || info.product_name () == "PID 117C"
	          )
        {
          sp = make_shared< WF_48xx > (cnx);
        }
      else
        {
          sp = make_shared< compound_scanner > (cnx);
        }
    }
  catch (const invalid_command& e)
    {
      log::brief
        ("does not appear to be an ESC/I-2 device (%1%)")
        % e.what ();
    }
  catch (const unknown_reply&)
    {
      // log::error
    }

  if (!sp)
    {
      try
        {
          get_status ESC_F;

          *cnx << ESC_F;
          if (ESC_F.supports_extended_commands ())
            {
              get_extended_status ESC_f;

              *cnx << ESC_f;

              log::brief ("detected a '%1%'") % ESC_f.product_name ();

              /**/ if (   ESC_f.product_name () == "GT-S650"
                       || ESC_f.product_name () == "Perfection V19"
                       || ESC_f.product_name () == "Perfection V39")
                {
                  sp = make_shared< GT_S650 > (cnx);
                }
              else
                {
                  sp = make_shared< extended_scanner > (cnx);
                }
            }
#ifdef HAVE_STANDARD_SCANNER
          else
            {
              sp = make_shared< standard_scanner > (cnx);
            }
#endif
        }
      catch (const invalid_command& e)
        {
          log::brief
            ("does not appear to be an ESC/I device (%1%)")
            % e.what ();
        }
      catch (const unknown_reply&)
        {
          // log::error
        }
      if (!sp) log::error ("not supported");
    }

  if (sp)
    {
      sp->configure ();
      rv = sp;
    }
}

} // extern "C"

namespace _drv_ {
namespace esci {

// Placeholder strings
// These are not yet used but we have translations for them.

const string adf_double_sided (SEC_N_("ADF - Double-sided"));
const string adf_single_sided (SEC_N_("ADF - Single-sided"));
const string monochrome (SEC_N_("Black & White"));
const string color (SEC_N_("Color"));
const string mode (SEC_N_("Mode"));
const string text_line_art (SEC_N_("Text/Line Art"));

scanner::scanner (const connexion::ptr& cnx)
  : utsushi::scanner (cnx)
  , gamma_exponent_(1)
{
  matrix< double, 3 >& mat
    = const_cast< matrix< double, 3 >& > (profile_matrix_);

  // TODO add a unit or diagonal matrix ctor so we can do this in the
  //      initializer list
  for (size_t i = 0; i < mat.rows (); ++i)
    mat[i][i] = 1;
}

bool
scanner::set_up_sequence ()
{
  val_ = values ();

  set_up_initialize ();

  set_up_doc_source ();
  set_up_image_mode ();

  set_up_gamma_tables ();
  set_up_color_matrices ();

  set_up_auto_area_segmentation ();
  set_up_threshold ();
  set_up_dithering ();

  set_up_sharpness ();
  set_up_brightness ();

  set_up_mirroring ();

  set_up_scan_speed ();
  set_up_scan_count ();

  set_up_resolution ();
  set_up_scan_area ();

  set_up_transfer_size ();

  return set_up_hardware ();
}

void
scanner::set_up_initialize ()
{}

bool
scanner::set_up_hardware ()
{
  return true;
}

}       // namespace esci
}       // namespace _drv_
}       // namespace utsushi
