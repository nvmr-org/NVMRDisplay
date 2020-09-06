node {
	stage('Clean'){
			cleanWs()
	}
	stage('Checkout'){
			fileOperations([folderCreateOperation('source')])
			dir('source'){
				def scmVars = checkout scm
				env.GIT_COMMIT = scmVars.GIT_COMMIT
			}
	}
	stage("Build"){
		if( repoHook.length() > 0 ){
			configFileProvider([configFile(fileId: "${repoHook}", targetLocation: 'hookdir/D21-repo-hook')]){
				//buildDebPkg_fn( arch, distro )
			}
		}else{
		//	buildDebPkg_fn( arch, distro )
		}

		debianPbuilder additionalBuildResults: '', 
			architecture: armhf, 
			components: '', 
			distribution: buster, 
			keyring: '', 
			mirrorSite: 'http://raspbian.rapsberrypi.org/raspbian', 
			pristineTarName: ''
	} //stage

	stage("Add to repo if master"){
		if( env.BRANCH_NAME == "master" ){
			//aptlyPublish includeSource: true, removeOldPackages: true, repositoryName: "nightly-nvmr"
		}
	}
}
