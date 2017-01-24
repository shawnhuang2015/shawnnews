'use strict';

var fs = require("fs");
var gulp = require('gulp');
var jshint = require('gulp-jshint');
var concat = require('gulp-concat');
var uglify = require('gulp-uglify');
var rename = require('gulp-rename');

var env = process.env.NODE_ENV || 'development';
/*
 var defaultTasks = ['clean', 'jshint', 'csslint','serve','watch']; // initialize with development settings
 if (env === 'production') { var defaultTasks = ['clean', 'cssmin', 'uglify', 'serve', 'watch'];}
 if (env === 'test')       { var defaultTasks = ['env:test', 'karma:unit', 'mochaTest'];}
 */
// read gulp directory contents for the tasks...
require('require-dir')('./gulp');
console.log('Invoking gulp -',env);
gulp.task('default', ['clean'], function (defaultTasks) {
    // run with paramater

    gulp.start(env);
});

gulp.task('jshint', function() {
    return gulp.src('packages/custom/cipmanager/server/*/*.js')
        .pipe(jshint())
        .pipe(jshint.reporter('default'));
});

gulp.task('minify', function() {
    var path = 'packages/custom/cipmanager/server';
    var dirList = fs.readdirSync(path);
    console.log('dirList: ' + dirList);
    dirList.forEach(function(item) {
        var npath = path + '/' + item;
        if(fs.statSync(npath).isDirectory()){
            var fileList = fs.readdirSync(npath);
            //console.log('fileList: ' + fileList);
            fileList.forEach(function(file) {
                var filename = npath + '/' + file;
                if (filename.indexOf(".js") != -1 && fs.statSync(filename).isFile) {
                    console.log('filename: ' + filename);

                    gulp.src(filename)
                        .pipe(uglify())
                        .pipe(gulp.dest(npath));

                }
            });
        }
    });

});


