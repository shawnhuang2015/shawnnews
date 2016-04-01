'use strict';

import mongoose from 'mongoose';

var SingleCrowdSchema = new mongoose.Schema({
  name: String,
  info: String,
  active: Boolean
});

export default mongoose.model('SingleCrowd', SingleCrowdSchema);
