//
// Build project for emscripten
// Run using:
// node make.js [-f FILE, --file=FILE, --makefile=FILE] [target]
//

var fs = require('fs');
var pathSep = require('path').sep;
var util = require('util'),
    exec = require('child_process').exec;

var start = new Date().getTime();

var makefileName = '.'+pathSep+'Makefile';

var makefile = require(makefileName);

var verbose = false;

var globalId = 0;

for (var i=0;i<process.argv.length;i++){
    if (process.argv[i] === '-f' && i+1 < process.argv.length){
        makefileName = process.argv[i+1];
    } else if (process.argv[i] === '-v' || process.argv[i] === '--verbose'){
        verbose = true;
    } else {
        var splitString = process.argv[i].split('=');
        if (splitString.length == 2 && (splitString[0] === '--file' || splitString[0] === '--makefile')){
            makefileName = splitString[1];
        }
    }
}

// https://gist.github.com/danherbert-epam/3960169
var mkdirSync = function __directory_mkdirSync__(path) {

    var dirs = path.split(pathSep);
    var root = "";

    while (dirs.length > 0) {
        var dir = dirs.shift();
        if (dir === "") {// If directory starts with a /, the first path will be an empty string.
            root = pathSep;
        }
        if (!fs.existsSync(root + dir)) {
            fs.mkdirSync(root + dir);
        }
        root += dir + pathSep;
    }
};

// https://gist.github.com/danherbert-epam/3960169
var mkdir = function __directory_mkdir__(path, callback) {
    var dirs = path.split(pathSep);
    var root = "";

    mkDir();

    function mkDir(){
        var dir = dirs.shift();
        if (dir === "") {// If directory starts with a /, the first path will be an empty string.
            root = pathSep;
        }
        fs.exists(root + dir, function(exists){
            if (!exists){
                fs.mkdir(root + dir, function(err){
                    root += dir + pathSep;
                    if (dirs.length > 0) {
                        mkDir();
                    } else if (callback) {
                        callback();
                    }
                });
            } else {
                root += dir + pathSep;
                if (dirs.length > 0) {
                    mkDir();
                } else if (callback) {
                    callback();
                }
            }
        });
    }
};

function relativePathToAbsolutePath(path){
    var pwd = process.cwd();
    while (path.substr(0,2) == ".."){
        path = path.substr(3); // skip "../"
        pwd = pwd.substr(0, pwd.lastIndexOf(pathSep));
    }
    return pwd + pathSep + path;
}

var getFilesUsingFilterResursive = function getFilesUsingFilterSelf(root, includeFn) {
    var filesUnfiltered = fs.readdirSync(root);
    var files = [];
    for (var i=0;i<filesUnfiltered.length;i++){
        var file = root+pathSep+filesUnfiltered[i];
        var stat = fs.statSync(file);
        if (stat.isDirectory()){
            var recursiveFiles = getFilesUsingFilterSelf(file, includeFn);
            Array.prototype.push.apply(files, recursiveFiles);
        } else if (includeFn(file)){
            files.push(file);
        }
    }
    return files;
}

var getFilesUsingFilter = function(filter){
    var root = filter.root || ".";
    if (root[root.length-1] == pathSep){
        root = root.substr(0,root.length-1);
    }
    var includeFilter = function(){return false;}
    if (typeof(filter.filter) === "string"){ // is regexp object
        var re = new RegExp(filter.filter);
        includeFilter = function(filename){return re.test(filename);};
    } else if (typeof(filter.filter) === "function"){
        includeFilter = filter.filter;
    }

    return getFilesUsingFilterResursive(root, includeFilter);
}

// http://stackoverflow.com/a/14387791/420250
function copyFile(source, target, cb) {
    if (verbose){
        console.log("Copy file from "+source+" to "+target);
    }
    var cbCalled = false;

    var rd = fs.createReadStream(source);
    rd.on("error", function(err) {
        done(err);
    });
    var wr = fs.createWriteStream(target);
    wr.on("error", function(err) {
        done(err);
    });
    wr.on("close", function(ex) {
        done();
    });
    rd.pipe(wr);

    function done(err) {
        if (!cbCalled) {
            cbCalled = true;
            cb(err);
        }
    }
}

var copyFilesUsingFilter = function(filter, destDir, excludeList, onFinish){
    var files = getFilesUsingFilter(filter);
    var root = filter.root || ".";
    if (root[root.length-1] != pathSep){
        root += pathSep;
    }
    if (destDir[destDir.length-1] != pathSep ){
        destDir += pathSep;
    }
    var copiedFiles = [];

    var queue = [];
    for (var i = 0;i<files.length;i++) {
        var file = files[i];
        var relativeFileName = file.substr(root.length);
        var lastPathSepPos = relativeFileName.lastIndexOf(pathSep);
        var relativePathName = lastPathSepPos !== -1 ? relativeFileName.substr(0, lastPathSepPos) : "";
        mkdirSync(destDir + relativePathName);
        if (excludeList.indexOf(relativeFileName) == -1){
            copiedFiles.push(relativeFileName);
            queue.push({"file":file,"relativeFileName":relativeFileName});
        } else if (verbose) {
            console.log("Skipping file "+file);
        }
    }
    if (queue.length == 0){
        onFinish([]);
        return;
    }

    var currentFile = 0;
    handleFile= function () {
        if (!queue[currentFile]){
            console.log(queue, currentFile, queue.length);
        }
        copyFile(queue[currentFile].file, destDir+queue[currentFile].relativeFileName, function(err){
            if (err){
                console.log(err);
            }
            currentFile++;
            if (currentFile > queue.length){
                console.log("Error too many callbacks for file. was "+currentFile+" queue length "+queue.length);
            }
            if (currentFile == queue.length){
                onFinish(copiedFiles);
            } else {
                handleFile();
            }
        });
    };
    handleFile();

}

function replaceAll(find, replace, str) {
    return str.replace(new RegExp(find, 'g'), replace);
}

function fixPath(path){
    return replaceAll("/", pathSep, path);
}


function queueExecs(commands, onEnd, cwd){
    var maxSimultaniously = 16;
    var executed = 0;
    var totalCommands = commands.length;
    if (commands.length>maxSimultaniously){
        var lastCommands = commands.slice(maxSimultaniously);
        var oldOnEnd = onEnd;
        var newOnEnd = function () {
            queueExecs(lastCommands, oldOnEnd, cwd);
        }
        onEnd = newOnEnd;
        commands = commands.slice(0,maxSimultaniously);
    }
    if (verbose){
        console.log("Batching "+commands.length+" new jobs to queue");
    }
    console.log(commands.length+"/"+totalCommands);

    for (var i=0;i<commands.length;i++){
        (function(){
            var command = commands[i];
            var options = { encoding: 'utf8',
                timeout: 0,
                maxBuffer: 200*1024,
                killSignal: 'SIGTERM',
                cwd: null,
                env: null };
            if (cwd){
                options.cwd = cwd;
            }
            if (verbose){
                console.log(command);
            }
            exec(command, options,
                function (error, stdout, stderr) {
                    if (stdout.length>0) {
                        console.log('stdout:\n' + stdout);
                    }
                    if (stderr.length>0) {
                        console.log('stderr:\n' + stderr);
                    }
                    if (error !== null) {
                        console.log('exec error:\n' + error);
                    }
                    executed++;
                    if (executed == commands.length){
                        onEnd();
                    }
                });
        })();

    }
}

function updatePaths(project){
    for (var i=0;i<project.includedirs.length;i++){
        project.includedirs[i] = relativePathToAbsolutePath(project.includedirs[i])
    }
    project.targetdir = relativePathToAbsolutePath(project.targetdir);
}

var BuildEM = function(project){
    // http://stackoverflow.com/a/12761924/420250
    var deleteFolderRecursive = function(path) {
        if (verbose){
            console.log("delete folder "+path);
        }
        var files = [];
        if( fs.existsSync(path) ) {
            files = fs.readdirSync(path);
            files.forEach(function(file,index){
                var curPath = path + pathSep + file;
                if(fs.lstatSync(curPath).isDirectory()) { // recurse
                    deleteFolderRecursive(curPath);
                } else { // delete file
                    fs.unlinkSync(curPath);
                }
            });
            fs.rmdirSync(path);
        }
    };


    updatePaths(project);
    var objectfiledir = fixPath(project.targetdir) + pathSep + "tmp";

    var clean = function(){
        deleteFolderRecursive(objectfiledir);
    }
    clean();

    var target = project.targetdir + pathSep + project.target;
    mkdir(objectfiledir);
    var allCommands = [];

    var options = "";
    for (var i=0;i<project.options.length;i++){
        options += project.options[i]+" ";
    }
    for (var i=0;i<project.includedirs.length;i++){
        options += "-I "+project.includedirs[i]+" ";
    }
    var buildoptions = "";
    for (var i=0;i<project.buildoptions.length;i++){
        buildoptions += project.buildoptions[i]+" ";
    }
    var defines = "";
    for (var i=0;i<project.defines.length;i++){
        defines += "-D "+project.defines[i]+" ";
    }
    var bcfiles = [];
    for (var i=0;i<project.files.length;i++){
        var files = getFilesUsingFilter(project.files[i]);
        for (var j=0;j<files.length;j++) {
            var file = files[j];
            var filename = file.split(pathSep).slice(-1)[0];
            var bcfilename = globalId+"___"+filename+".bc";
            globalId++;
            var bcfile = objectfiledir+pathSep + bcfilename;
            var command = "emcc " + options +buildoptions+ defines + file + " -o " + bcfile;
            allCommands.push(command);
            bcfiles.push(bcfilename);
        }
    }


    var link = function(){
        var command = "emcc " + options + buildoptions;
        for (var i=0;i<bcfiles.length;i++){
            command += bcfiles[i] + " ";
        }
        var finish = function(){
            if (verbose){
                console.log("Linking");
            }
            command += " -o " + target;
            var onExit = function(){
                clean();
                var end = new Date().getTime();
                var time = end - start;
                console.log("Finished at "+new Date()+" in "+Math.floor(time/(1000*60))+" minutes "+Math.floor((time/1000)%60)+" seconds");
            };
            queueExecs([command], onExit , objectfiledir);
        }

        var skipEmbedFiles = [];
        var embedCount = 0;
        var embed = function(){

            copyFilesUsingFilter(project.embed[embedCount], objectfiledir, skipEmbedFiles, function(files){
                for (var j=0;j<files.length;j++){
                    command += "--embed-file "+files[j]+" ";
                }
                embedCount++;
                if (embedCount == project.embed.length){
                    finish();
                } else {
                    embed();
                }
            });

        }

        if (verbose){
            console.log("Searching for preload files");
        }
        var preloadCount = 0;
        function preloadFiles(){
            copyFilesUsingFilter(project.preload[preloadCount], objectfiledir, [], function(files){
                for (var j=0;j<files.length;j++){
                    command += "--preload-file "+files[j]+" ";
                    skipEmbedFiles.push(files[j]);
                }
                preloadCount++;
                if (preloadCount == project.preload.length){
                    if (verbose){
                        console.log("Searching for embed files");
                    }
                    embed();
                } else {
                    preloadFiles();
                }
            });
        }
        preloadFiles();
    };

    queueExecs(allCommands, link);
};


BuildEM(makefile);