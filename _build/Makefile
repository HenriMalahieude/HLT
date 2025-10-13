CC = gcc
AR = ar
BFLAGS = -Wall
DY_FLAGS = -fPIC

# Default to Lexic

NAME ?= Lexic
$(eval REMAINDDER_NAME := $$$(NAME))
ABRV :=$(subst $(REMAINDDER_NAME),,$(NAME))

# If you have a better idea, I'm all ears.
# No. I am not installing GNU/Make Standard Lib
LOWER_NAME := $(shell echo $(NAME) | tr A-Z a-z)
LIBNAME ?=$(foreach t,$(LOWER_NAME),lib$t)

OUTDIR ?= ./Lexic
OUTLIB ?= ./lib
SRC_LOC ?= ../Lexic/private
HLP_LOC ?= ../helpers
HLP_OBJDIR ?= ./Helpers
STATIC_OUTDIR := $(OUTDIR)/object
DYNAMI_OUTDIR := $(OUTDIR)/shared

HLP_HDRS := $(shell find $(HLP_LOC) -name '*.h')
SRC_HDRS := $(shell find $(SRC_LOC)/.. -name '*.h')

SOURCES := $(wildcard $(SRC_LOC)/*.c)
OBJECTS := $(foreach t, $(SOURCES),$(basename $(notdir $t)).o)

DYN_OBJS := $(foreach t, $(OBJECTS),$(DYNAMI_OUTDIR)/$t)
STT_OBJS := $(foreach t, $(OBJECTS),$(STATIC_OUTDIR)/$t)

# Print Help
NULL :=
TAB := $(NULL)    $(NULL)
define ENDL


endef

.PHONY: all dynamic static 

all: $(HLP_OBJDIR) dynamic static 
	$(info [!!$(ABRV)] $(NAME) Library Fully Compiled$(ENDL))

dynamic: $(DYNAMI_OUTDIR) $(OUTLIB)/$(LIBNAME).so
	$(info [!d$(ABRV)]$(TAB)$(NAME) Dynamically Complete)

static: $(STATIC_OUTDIR) $(OUTLIB)/$(LIBNAME).a
	$(info [!s$(ABRV)]$(TAB)$(NAME) Statically Complete)

# Dynamic Library Creation
$(OUTLIB)/%.so: $(DYN_OBJS)
	$(info [dl$(ABRV)]$(TAB)Linking dynamically!)
	$(CC) -shared -o $@ $(DYNAMI_OUTDIR)/*.o $(HLP_OBJDIR)/shared/*.o $(BFLAGS) $(DY_FLAGS)

$(DYNAMI_OUTDIR)/%.o: $(SRC_LOC)/%.c $(SRC_HDRS) $(HLP_HDRS)
	$(info [.d$(ABRV)]$(TAB)$(TAB)$(notdir $@))
	$(CC) $< -c -o $@ $(BFLAGS) $(DY_FLAGS) -I $(HLP_LOC)

# Static Library Creation
$(OUTLIB)/%.a: $(STT_OBJS)
	$(info [sl$(ABRV)]$(TAB)Linking lexic statically!)
	$(AR) rcs $@ $(STATIC_OUTDIR)/*.o $(HLP_OBJDIR)/object/*.o

$(STATIC_OUTDIR)/%.o: $(SRC_LOC)/%.c $(SRC_HDRS) $(HLP_HDRS)
	$(info [.o$(ABRV)]$(TAB)$(TAB)$(notdir $@))
	$(CC) $< -c -o $@ $(BFLAGS) -I $(HLP_LOC)

$(STATIC_OUTDIR) $(DYNAMI_OUTDIR):
	mkdir -p $@

$(HLP_OBJDIR):
	$(info [XXX] Helper Objects weren't created? $(ENDL) $(ENDL))
	$(error No Helper Objects)
