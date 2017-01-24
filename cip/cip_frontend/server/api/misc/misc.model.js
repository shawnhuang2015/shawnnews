'use strict';

import mongoose from 'mongoose';

var MiscSchema = new mongoose.Schema({
  name: String,
  info: String,
  active: Boolean
});

export default mongoose.model('Misc', MiscSchema);
