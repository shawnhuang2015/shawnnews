import { AppsPage } from './app.po';

describe('apps App', function() {
  let page: AppsPage;

  beforeEach(() => {
    page = new AppsPage();
  });

  it('should display message saying app works', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('app works!');
  });
});
