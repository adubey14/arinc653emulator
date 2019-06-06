SUBDIRS =
DR_FOLDER=
DR_PC=
if HAVE_DOXYGEN
SUBDIRS += @extra_compile_dir@ src  ModuleManager NetworkPartition  StateMachine ${DR_FOLDER} docs
#Distribute these directories:
DIST_SUBDIRS =  src ModuleManager NetworkPartition  test StateMachine docs
else
SUBDIRS += @extra_compile_dir@ src  ModuleManager NetworkPartition StateMachine ${DR_FOLDER}
#Distribute these directories:
DIST_SUBDIRS =  src ModuleManager NetworkPartition test StateMachine 
endif
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ACM-1.0.pc ${DR_PC}
dist-hook:
	rm -rf `find $(distdir) -name .svn`
	popd
dist:
	@echo "Use make dist-zip"
	
if generateciaotypes
install-data-local:
		$(mkinstalldirs) $(prefix)
		$(mkinstalldirs) $(prefix)/bin
		$(mkinstalldirs) $(prefix)/MPC
		$(mkinstalldirs) $(prefix)/MPC/config
		$(INSTALL_DATA) MPC/config/* $(prefix)/MPC/config/.
		cp utilityscripts/* $(prefix)/bin/.
		chmod +x $(prefix)/bin/*
		pushd $(top_srcdir)/arincdemo;\
                for d in `find . -type d |grep -v '.*svn'`; do \
                $(mkinstalldirs) $(prefix)/arincdemo/$$d; \
                for file in $$d/*; do \
                $(INSTALL_DATA) $$file $(prefix)/arincdemo/$$d/.; \
                done; \
                done; \
                popd;
		$(INSTALL_DATA) LICENSE.* $(prefix)/.
		@echo "#------ Creating $(prefix)/bootstrap.sh--------"
		@echo "export ACM_ROOT=$(prefix)">$(prefix)/bootstrap.sh
		echo "export MBSHM_ROOT=$(prefix)">>$(prefix)/bootstrap.sh
		echo "export ACE_ROOT=$(ACE_ROOT)">>$(prefix)/bootstrap.sh
		echo "export MPC_ROOT=$(MPC_ROOT)">>$(prefix)/bootstrap.sh
		echo "export TAO_ROOT=$(TAO_ROOT)">>$(prefix)/bootstrap.sh
		echo "export CIAO_ROOT=$(CIAO_ROOT)">>$(prefix)/bootstrap.sh
		echo "export PATH=\$$PATH:\$$MBSHM_ROOT/bin:\$$ACE_ROOT/bin">>$(prefix)/bootstrap.sh
		echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:\$$MBSHM_ROOT/lib:\$$ACE_ROOT/lib">>$(prefix)/bootstrap.sh
if generateddstypes	
		echo "export DDS_ROOT=$(DDS_ROOT)">>$(prefix)/bootstrap.sh
		echo "export PATH=\$$PATH:\$$DDS_ROOT/bin">>$(prefix)/bootstrap.sh		
		echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:\$$DDS_ROOT/lib">>$(prefix)/bootstrap.sh
endif
		echo "export PKG_CONFIG_PATH=\$$MBSHM_ROOT/lib/pkgconfig/" >>$(prefix)/bootstrap.sh
		chmod +x $(prefix)/bootstrap.sh
		chmod +x `find $(prefix)/ -name 'autogen.sh'`
		@echo "----------------------------------------------------------------------"
		@echo
		@echo " A pkg_config file is being installed in $(libdir)/pkgconfig "
		@echo " Put the export PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$(libdir)/pkgconfig command in you .profile or .bashrc"
		@echo " $(prefix)/arincdemo contains examples on how to use pkg-config for linking against libraries"
		@echo " see man pkg-config more information about pkg-config"
		@echo
		@echo "----------------------------------------------------------------------"

else
install-data-local: 
		$(mkinstalldirs) $(prefix)
		$(mkinstalldirs) $(prefix)/bin
		cp utilityscripts/* $(prefix)/bin/.
		chmod +x $(prefix)/bin/*
		$(mkinstalldirs) $(prefix)/arincdemo 
		pushd $(top_srcdir)/arincdemo;\
		for d in `find . -type d |grep -v '.*svn'`; do \
		$(mkinstalldirs) $(prefix)/arincdemo/$$d; \
		for file in $$d/*; do \
		$(INSTALL_DATA) $$file $(prefix)/arincdemo/$$d/.; \
		done; \
		done; \
		popd; 
		$(INSTALL_DATA) LICENSE.* $(prefix)/.

		@echo "#------ Creating $(prefix)/bootstrap.sh--------"
		@echo "export ACM_ROOT=$(prefix)">$(prefix)/bootstrap.sh
		echo "export MBSHM_ROOT=$(prefix)">>$(prefix)/bootstrap.sh
		echo "export PATH=\$$PATH:\$$MBSHM_ROOT/bin">>$(prefix)/bootstrap.sh
		echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:\$$MBSHM_ROOT/lib">>$(prefix)/bootstrap.sh
		echo "export PKG_CONFIG_PATH=\$$MBSHM_ROOT/lib/pkgconfig/" >>$(prefix)/bootstrap.sh
		chmod +x $(prefix)/bootstrap.sh
		chmod +x $(prefix)/arincdemo/autogen.sh
		@echo "----------------------------------------------------------------------"
		@echo
		@echo " A pkg_config file is being installed in $(libdir)/pkgconfig "
		@echo " Put the export PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$(libdir)/pkgconfig command in you .profile or .bashrc"
		@echo " $(prefix)/arincdemo contains examples on how to use pkg-config for linking against libraries"
		@echo " see man pkg-config more information about pkg-config"		           
		@echo 
		@echo "----------------------------------------------------------------------"
endif
EXTRA_DIST = utilityscripts arincdemo include LICENSE.pdf LICENSE.txt autogen.sh 
realclean: clean
