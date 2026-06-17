'use strict';
const Joi = require('joi');

/**
 * Factory: returns an Express middleware that validates req.body
 * against a Joi schema. Passes a 400 error to next() on failure.
 */
module.exports = (schema) => (req, res, next) => {
  const { error, value } = schema.validate(req.body, { abortEarly: false, stripUnknown: true });
  if (error) {
    const details = error.details.map(d => d.message);
    return res.status(400).json({ error: 'Validation failed', details });
  }
  req.body = value;   // use sanitised value
  next();
};
