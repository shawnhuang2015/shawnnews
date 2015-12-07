/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
(function(undefined) {
  "use strict";

  console.log('Loading gvis.settings')

  var settings = {
    nodeKeyConcChar : '-'
    ,linkKeyConcChar : '_'
    ,attrs: '_attrs'
    ,styles: '_styles'
    ,key: '_key'
  }

  gvis.settings = gvis.utils.extend(gvis.settings || {}, settings);
