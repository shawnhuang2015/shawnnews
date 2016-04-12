/**
 * Using Rails-like standard naming convention for endpoints.
 * GET     /api/miscs/download     ->  download
 */

'use strict';

import _ from 'lodash';
import Misc from './misc.model';
import config from '../../config/environment';

/**
 * Download the requested file.
 * @param  req [request object]
 * @param  res [response object]
 */
export function download(req, res) {
  var file = req.query.file;
  res.download(config.dataPath + '/' + file);
}
