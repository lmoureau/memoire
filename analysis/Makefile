
all: raw calo notcalo custom
.PHONY: all raw calo notcalo custom

export PATH := ..:$(PATH)

define process
$(strip $(3)).events: $(strip $(1)).events $(2);
	process $(2) >$(strip $(3)).events <$(strip $(1)).events
endef

define process_not
$(strip $(3)).events: $(strip $(1)).events $(2);
	process not $(2) >$(strip $(3)).events <$(strip $(1)).events
endef

define accumulate
$(strip $(3)).hist: $(strip $(1)).events $(2);
	accumulate $(strip $(2)) >$(strip $(3)).hist <$(strip $(1)).events
endef

#
# Process
#

## BASIC ##

out.events:
	readroot data/out.root >out.events

$(call process, out, good_tracks.lua, out.good_tracks)

## TRACKS ##

$(call process, out.good_tracks, select2tracks.lua, out.2tracks)
$(call process, out.good_tracks, select3tracks.lua, out.3tracks)
$(call process, out.good_tracks, select4tracks.lua, out.4tracks)

## RHO ##

$(call process, out.2tracks, rhomaker.lua, out.2tracks.rho)
$(call process, out.3tracks, rhomaker.lua, out.3tracks.rho)
$(call process, out.4tracks, rhomaker.lua, out.4tracks.rho)

## OS/SS ##

$(call process,     out.2tracks.rho, neutral.lua, out.2tracks.os)
$(call process_not, out.2tracks.rho, neutral.lua, out.2tracks.ss)
$(call process,     out.4tracks.rho, neutral.lua, out.4tracks.neutral)

## CALO ##

$(call process, out.2tracks.os, calocut.lua, out.2tracks.os.calo)
$(call process, out.2tracks.ss, calocut.lua, out.2tracks.ss.calo)
$(call process, out.3tracks.rho, calocut.lua, out.3tracks.calo)
$(call process, out.4tracks.neutral, calocut.lua, out.4tracks.calo)

## NOT CALO ##

$(call process_not, out.2tracks.os, calocut.lua, out.2tracks.os.not.calo)
$(call process_not, out.2tracks.ss, calocut.lua, out.2tracks.ss.not.calo)
$(call process_not, out.3tracks.rho, calocut.lua, out.3tracks.not.calo)
$(call process_not, out.4tracks.neutral, calocut.lua, out.4tracks.not.calo)

#
# Fills
#

## RAW ##

raw: out.2tracks.hist \
     out.2tracks.os.hist \
     out.2tracks.ss.hist \
     out.3tracks.hist \
     out.4tracks.hist

$(call accumulate, out.2tracks.rho, rhofiller.lua, out.2tracks)
$(call accumulate, out.2tracks.os, rhofiller.lua, out.2tracks.os)
$(call accumulate, out.2tracks.ss, rhofiller.lua, out.2tracks.ss)
$(call accumulate, out.3tracks.rho, rhofiller.lua, out.3tracks)
$(call accumulate, out.4tracks.neutral, rhofiller.lua, out.4tracks)

## CALO ##

calo: out.2tracks.os.calo.hist \
      out.2tracks.ss.calo.hist \
      out.3tracks.calo.hist \
      out.4tracks.calo.hist

$(call accumulate, out.2tracks.os.calo, rhofiller.lua, out.2tracks.os.calo)
$(call accumulate, out.2tracks.ss.calo, rhofiller.lua, out.2tracks.ss.calo)
$(call accumulate, out.3tracks.calo, rhofiller.lua, out.3tracks.calo)
$(call accumulate, out.4tracks.calo, rhofiller.lua, out.4tracks.calo)

## NOT CALO ##

notcalo: out.2tracks.os.not.calo.hist \
         out.2tracks.ss.not.calo.hist \
         out.3tracks.not.calo.hist \
         out.4tracks.not.calo.hist

$(call accumulate, out.2tracks.os.not.calo, rhofiller.lua, out.2tracks.os.not.calo)
$(call accumulate, out.2tracks.ss.not.calo, rhofiller.lua, out.2tracks.ss.not.calo)
$(call accumulate, out.3tracks.not.calo, rhofiller.lua, out.3tracks.not.calo)
$(call accumulate, out.4tracks.not.calo, rhofiller.lua, out.4tracks.not.calo)

## CUSTOM ##

custom: out.2tracks.os.calo.ct.hist \
        out.2tracks.os.not.calo.ct.hist \
        out.3tracks.calo.ct.hist \
        out.3tracks.not.calo.ct.hist \
        out.4tracks.calo.ct.hist \
        out.4tracks.not.calo.ct.hist

$(call accumulate, out.2tracks.os.calo, calo_track.lua, out.2tracks.os.calo.ct)
$(call accumulate, out.2tracks.os.not.calo, calo_track.lua, out.2tracks.os.not.calo.ct)
$(call accumulate, out.3tracks.calo, calo_track.lua, out.3tracks.calo.ct)
$(call accumulate, out.3tracks.not.calo, calo_track.lua, out.3tracks.not.calo.ct)
$(call accumulate, out.4tracks.calo, calo_track.lua, out.4tracks.calo.ct)
$(call accumulate, out.4tracks.not.calo, calo_track.lua, out.4tracks.not.calo.ct)
