
all: raw calo notcalo custom
.PHONY: all raw calo notcalo custom

export PATH := ../build:../buildi7:$(PATH)

define process
data/$(strip $(3)).events: data/$(strip $(1)).events $(2);
	process $(2) >data/$(strip $(3)).events <data/$(strip $(1)).events
endef

define process_not
data/$(strip $(3)).events: data/$(strip $(1)).events $(2);
	process not $(2) >data/$(strip $(3)).events <data/$(strip $(1)).events
endef

define accumulate
data/$(strip $(3)).hist: data/$(strip $(1)).events $(2);
	accumulate $(strip $(2)) >data/$(strip $(3)).hist <data/$(strip $(1)).events
endef

#
# Process
#

## BASIC ##

data/out.events:
	readroot data/out.root >data/out.events

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

raw: data/out.2tracks.hist \
     data/out.2tracks.os.hist \
     data/out.2tracks.ss.hist \
     data/out.3tracks.hist \
     data/out.4tracks.hist

$(call accumulate, out.2tracks.rho, rhofiller.lua, out.2tracks)
$(call accumulate, out.2tracks.os, rhofiller.lua, out.2tracks.os)
$(call accumulate, out.2tracks.ss, rhofiller.lua, out.2tracks.ss)
$(call accumulate, out.3tracks.rho, rhofiller.lua, out.3tracks)
$(call accumulate, out.4tracks.neutral, rhofiller.lua, out.4tracks)

## CALO ##

calo: data/out.2tracks.os.calo.hist \
      data/out.2tracks.ss.calo.hist \
      data/out.3tracks.calo.hist \
      data/out.4tracks.calo.hist

$(call accumulate, out.2tracks.os.calo, rhofiller.lua, out.2tracks.os.calo)
$(call accumulate, out.2tracks.ss.calo, rhofiller.lua, out.2tracks.ss.calo)
$(call accumulate, out.3tracks.calo, rhofiller.lua, out.3tracks.calo)
$(call accumulate, out.4tracks.calo, rhofiller.lua, out.4tracks.calo)

## NOT CALO ##

notcalo: data/out.2tracks.os.not.calo.hist \
         data/out.2tracks.ss.not.calo.hist \
         data/out.3tracks.not.calo.hist \
         data/out.4tracks.not.calo.hist

$(call accumulate, out.2tracks.os.not.calo, rhofiller.lua, out.2tracks.os.not.calo)
$(call accumulate, out.2tracks.ss.not.calo, rhofiller.lua, out.2tracks.ss.not.calo)
$(call accumulate, out.3tracks.not.calo, rhofiller.lua, out.3tracks.not.calo)
$(call accumulate, out.4tracks.not.calo, rhofiller.lua, out.4tracks.not.calo)

## CUSTOM ##

custom: data/out.2tracks.os.calo.ct.hist \
        data/out.2tracks.os.not.calo.ct.hist \
        data/out.3tracks.calo.ct.hist \
        data/out.3tracks.not.calo.ct.hist \
        data/out.4tracks.calo.ct.hist \
        data/out.4tracks.not.calo.ct.hist

$(call accumulate, out.2tracks.os.calo, calo_track.lua, out.2tracks.os.calo.ct)
$(call accumulate, out.2tracks.os.not.calo, calo_track.lua, out.2tracks.os.not.calo.ct)
$(call accumulate, out.3tracks.calo, calo_track.lua, out.3tracks.calo.ct)
$(call accumulate, out.3tracks.not.calo, calo_track.lua, out.3tracks.not.calo.ct)
$(call accumulate, out.4tracks.calo, calo_track.lua, out.4tracks.calo.ct)
$(call accumulate, out.4tracks.not.calo, calo_track.lua, out.4tracks.not.calo.ct)
