#! /bin/bash
set -ex

orig_path=${ORIGINAL_PATH-"${PATH}"}
export PATH="/bin:/usr/bin"
export PATH="${PATH}:$(cygpath -u "${WINDIR}")/System32"

vcvarsall_native="${PROGRAMFILES} (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat"
vcvarsall="$(cygpath -ua "${vcvarsall_native}")"
setupopts=(--quiet --norestart --wait --includeRecommended)
buildopts=()
buildcmd=(cmd /c build_tesseract_vs2017.bat)
run_installer=1
toolsversion="15.0"
platformtoolset="v141"
windowstargetplatformversion="10.0.15063.0"
while (( $# )) ; do
    case "$1" in
        --installPath)
            run_installer=1
            instdir="$(cygpath -ua "$2")"
            instdir_native="$(cygpath -wa "$2")"
            shift
            setupopts=(--installPath "${instdir_native}" "${setupopts[@]}")
            vcvarsall_native="${instdir_native}\\VC\\Auxiliary\\Build\\vcvarsall.bat"
            vcvarsall="$(cygpath -ua "${vcvarsall_native}")"
            buildopts+=("${vcvarsall_native}")
            ;;
        --reinstall)
            run_installer=2
            ;;
        --vcvarsall)
            run_installer=0
            shift
            vcvarsall="$(cygpath -ua "$1")"
            vcvarsall_native="$(cygpath -wa "$1")"
            buildopts+=("${vcvarsall_native}")
            ;;
        --tools)
            shift
            toolsversion="$1"
            ;;
        --platform)
            shift
            platformtoolset="$1"
            ;;
        --target)
            shift
            windowstargetplatformversion="$1"
            ;;
        --fortify)
            buildopts+=(--fortify)
            ;;
        --coverity)
            buildopts+=(--coverity)
            ;;
    esac
    shift
done

# TODO: terminate only children of this script to avoid interference with concurrent builds
trap '
    err=$?
    taskkill /f /t /im msbuild.exe 2>&1 | sed -e "s#^ERROR: #taskkill: #" || :
    taskkill /f /t /im mspdbsrv.exe 2>&1 | sed -e "s#^ERROR: #taskkill: #" || :
    taskkill /f /t /im vs_buildtools.exe 2>&1 | sed -e "s#^ERROR: #taskkill: #" || :
    taskkill /f /t /im vctip.exe 2>&1 | sed -e "s#^ERROR: #taskkill: #" || :
    exit ${err}
    ' EXIT ERR SIGHUP SIGINT SIGTERM

if (( run_installer )) ; then
    if [[ -s "${vcvarsall}" ]] ; then
        echo "VS2017 build tools appear installed as this file exists: ${vcvarsall}"
    fi
    if [[ ! -s "${vcvarsall}" ]] || (( run_installer == 2 )) ; then
        vscomponents=(
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64"
            "Microsoft.VisualStudio.Component.VC.CLI.Support"
            "Microsoft.VisualStudio.Component.Windows10SDK"
            "Microsoft.VisualStudio.Component.Windows10SDK.15063.Desktop"
            "Microsoft.VisualStudio.Component.VC.CoreBuildTools"
        )
        # vsopts=("${vscomponents[@]/%/\;includeRecommended}")
        vsopts=("${vscomponents[@]}")
        vsopts=(${vsopts[@]/#/--add })

        test -s vs_BuildTools.exe \
            || curl -Lo "vs_BuildTools.exe" \
                "https://aka.ms/vs/15/release/vs_buildtools.exe"
                # "https://download.microsoft.com/download/7/8/5/78560ECB-5371-4CB6-AA6C-6D0978AC2332/vs_BuildTools.exe"
        chmod 0755 "vs_BuildTools.exe"
        ( 
            export PATH="${orig_path}"
            cmd /c vs_BuildTools.exe "${setupopts[@]}" "${vsopts[@]}"
        )
    fi
fi

# git submodule update --init --recursive

find -name "*.vcxproj" -exec bash -ec "
        echo \"{}\"
        sed -e \"s/\\\\(ToolsVersion=\\\\)\\\"14\\.0\\\"/\\1\\\"${toolsversion}\\\"/\" \
            -e \"s/\\\\(<PlatformToolset>\\\\)v140\\\\(<\\/PlatformToolset>\\\\)/\\1${platformtoolset}\\2/\" \
            -e \"s/\\\\(<WindowsTargetPlatformVersion>\\\\)8\\\\.1\\\\(<\\/WindowsTargetPlatformVersion>\\\\)/\\1${windowstargetplatformversion}\\2/\" \
            < \"{}\" > \"{}\".tmp
        if cmp -s \"{}\" \"{}\".tmp ; then rm -f \"{}\".tmp ; else { mv \"{}\".tmp \"{}\"; echo changed; } fi
    " \;

(
    export PATH="${orig_path}"
    "${buildcmd[@]}" "${buildopts[@]}"
)

