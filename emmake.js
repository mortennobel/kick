//
// Build project for emscripten
// Run using:
// node make.js [-f FILE, --file=FILE, --makefile=FILE] [target]
//

var makefileName = './Makefile';

var verbose = false;

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

var makefile = require(makefileName);
var fs = require('fs');
var pathSep = require('path').sep;
var util = require('util'),
    exec = require('child_process').exec;

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
            cb(err);
            cbCalled = true;
        }
    }
}

var copyFilesUsingFilter = function(filter, destDir, onFinish){
    var files = getFilesUsingFilter(filter);
    var root = filter.root || ".";
    if (root[root.length-1] != pathSep){
        root += pathSep;
    }
    if (destDir[destDir.length-1] != pathSep ){
        destDir += pathSep;
    }
    var copiedFiles = [];
    var copiedFilesCount = 0;
    for (var i = 0;i<files.length;i++){
        var file = files[i];
        var relativeFileName = file.substr(root.length);
        var lastPathSepPos = relativeFileName.lastIndexOf(pathSep);
        var relativePathName = lastPathSepPos !== -1 ? relativeFileName.substr(0, lastPathSepPos) : "";
        mkdirSync(destDir+relativePathName);
        copiedFiles.push(relativeFileName);
        copyFile(file, destDir+relativeFileName, function(err){
            if (err){
                console.log(err);
            }
            copiedFilesCount++;
            if (copiedFilesCount == files.length){
                onFinish(copiedFiles);
            }
        });
    }
}

function replaceAll(find, replace, str) {
    return str.replace(new RegExp(find, 'g'), replace);
}

function fixPath(path){
    return replaceAll("/", pathSep, path);
}


function queueExecs(commands, onEnd, cwd){
    var executed = 0;
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
            exec(command, options,
                function (error, stdout, stderr) {
                    if (verbose){
                        console.log(command);
                    }
                    if (stdout.length>0) {
                        console.log('stdout:\n' + stdout);
                    }
                    if (stderr.length>0) {
                        console.log('stderr:\n' + stderr);
                    }
                    if (error !== null) {
                        console.log('exec error:\n' + error);
                        process.exit(1);
                    }
                    executed++;
                    if (executed == commands.length){
                        onEnd();
                    }
                });
        })();

    }
}

var BuildEM = function(project){
    var objectfiledir = fixPath(project.targetdir) + pathSep + "tmp";
    var target = ".." + pathSep + project.target;
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
    for (var i=0;i<project.defines.length;i++){
        buildoptions += "-D "+project.defines[i]+" ";
    }
    var bcfiles = [];
    for (var i=0;i<project.files.length;i++){
        var files = getFilesUsingFilter(project.files[i]);
        for (var j=0;j<files.length;j++) {
            var file = files[j];
            var filename = file.split(pathSep).slice(-1)[0];
            var bcfile = objectfiledir+pathSep + filename+".bc";
            var command = "emcc " + options +buildoptions+ file + " -o " + bcfile;
            allCommands.push(command);
            bcfiles.push(filename +".bc");
        }
    }

    // http://stackoverflow.com/a/12761924/420250
    var deleteFolderRecursive = function(path) {
        var files = [];
        if( fs.existsSync(path) ) {
            files = fs.readdirSync(path);
            files.forEach(function(file,index){
                var curPath = path + "/" + file;
                if(fs.lstatSync(curPath).isDirectory()) { // recurse
                    deleteFolderRecursive(curPath);
                } else { // delete file
                    fs.unlinkSync(curPath);
                }
            });
            fs.rmdirSync(path);
        }
    };

    var clean = function(){
        if (verbose){
            console.log("Keeping object directory "+objectfiledir);
        } else {
            deleteFolderRecursive(objectfiledir);
        }
    }
    var link = function(){
        var command = "emcc " + options;
        for (var i=0;i<bcfiles.length;i++){
            command += bcfiles[i] + " ";
        }
        var count = 0;
        var finish = function(){
            count++;
            if (count == project.preload.length + project.embed.length){
                command += " -o " + target;
                queueExecs([command], clean, objectfiledir);
            }
        }

        for (var i=0;i<project.preload.length;i++) {
            copyFilesUsingFilter(project.preload[i], objectfiledir, function(files){
                for (var j=0;j<files.length;j++){
                    command += "--preload-file "+files[j]+" ";
                }
                finish();
            });
        }
        for (var i=0;i<project.embed.length;i++) {
            copyFilesUsingFilter(project.embed[i], objectfiledir, function(files){
                for (var j=0;j<files.length;j++){
                    command += "--embed-file "+files[j]+" ";
                }
                finish();
            });
        }
    };

    queueExecs(allCommands, link);
};


BuildEM(makefile);