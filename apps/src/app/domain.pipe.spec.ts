/* tslint:disable:no-unused-variable */

import { TestBed, async } from '@angular/core/testing';
import { DomainPipe } from './domain.pipe';

describe('DomainPipe', () => {
  it('create an instance', () => {
    let pipe = new DomainPipe();
    expect(pipe).toBeTruthy();
    expect(pipe.transform).toBeDefined();
    expect(pipe.transform('http://www.test.com/a/b?c;d=2')).toEqual('(test.com)');
    expect(pipe.transform('something wrong')).toEqual('');
    expect(pipe.transform(false)).toEqual('');
  });
});
