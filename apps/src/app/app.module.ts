import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';

import { MomentModule } from 'angular2-moment';

import { AppComponent } from './app.component';
import { HeaderComponent } from './header/header.component';
import { StoriesComponent } from './stories/stories.component';
import { FootComponent } from './foot/foot.component';
import { ItemComponent } from './stories/item/item.component';
import { NewsApiService } from './news-api.service';
import { DomainPipe } from './domain.pipe';
import { ItemCommentsComponent } from './stories/item/item-comments/item-comments.component';

import { APP_BASE_HREF } from '@angular/common';

import { RouterModule, Routes } from '@angular/router';
import { routing } from './app.routes';
import { CommentComponent } from './stories/item/item-comments/comment/comment.component';
import { CommentTreeComponent } from './stories/item/item-comments/comment-tree/comment-tree.component';


@NgModule({
  declarations: [
    AppComponent,
    HeaderComponent,
    StoriesComponent,
    FootComponent,
    ItemComponent,
    DomainPipe,
    ItemCommentsComponent,
    CommentComponent,
    CommentTreeComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    MomentModule,
    routing,
    RouterModule
  ],
  providers: [
    NewsApiService,
    { provide: APP_BASE_HREF, useValue: '/' }],
  bootstrap: [AppComponent]
})
export class AppModule { }
