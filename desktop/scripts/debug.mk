# 
# copyright 2019 NSWA Maintainers
# copyright 2019 The-Seth-Project
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

ifeq ($(SETHDKT_DEBUG),true)

# disable strip by use `true` command to replace strip
STRIP := true

# enable -g option for compiler
COMMENFLAGS += -g

# disable opt
FLAG_OPT := -O0

# disable lto
FLAG_LTO :=
FLAG_LD_LTO :=

endif
