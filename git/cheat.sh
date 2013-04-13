#!/usr/bin/env bash

##config

  #first of all steps: tell git who you are

  #others may see this on commits

    git config --global user.name "Ciro Duran Santilli"
    git config --global user.email "ciro.santilli@gmail.com"

##init

  #create a git repo with you existing code

  #starts a repository here:

    git init

  ##bare

    #starts a bare repo here:

      git init --bare

    #it this contains directly the contents of the .git file.

    #a repo must be bare if you want to push to it/pull from it

##clone

  #get a git repo from somewhere, local or remote:

    R=git@github.com:cirosantilli/bash.git
    git clone $R

  #copies files to your computer

  #automatically creates a remote location called origin

  #see remote for what that means

  ##bare

    #what github does on `fork`!

      git clone --bare $R

##remote

  #create aliases to a remote repo

  #when you clone something, it alreay has a origin remote.

  #create an alias to a remote repo:

    git remote add github https://github.com/cirosantilli/latex.git

  #shows remote repo aliases without their real addresses:

    git remote 

  #shows remote repo aliases and their real addresses:

    git remote -v

  #view all names of remote branches:

    git remote show

  #view detail of branch:

    git remote show $B

  #remove the branch github:

    git remote rm github

 #if you for instance entered the bad url, you will have to first
  #remove the old one before you can add the correct url)

##add files

  #if you don't add files, git won't see them 

    git add $F
  #add a file
    #on next commit it will be seen by git
    #if you do commit -a, git will check all files that have already been added
    #without -a, you have to add each time

    git add '*.txt'
  #RECURSIVE add

    ack --py f | xargs -I'{}' git add "{}" +
  #use ack f option to add lots of source files at once

##commit

  #commit is creating a version

  #see uncommited changes and untracked files:

    git status

    git commit -a -v

  #-a: commits all files that have been modified or deleted
    #(if deleted they will stop being tracked for the future)
    #without this, you have to add files each time one by one
  #-v: opens vim in a commit diff log. you can either enter a commit message on the first line,
  #  or leave it blank and cancel comit. action is taken after you save ans quit :wq

    git commit -am 'commit message'

  #same as above, but commits directly with message commit message

  ##ammend

    #correct last commibt *before it was pushed*:

    #does not create new commitmessage

    #correct message only:

      git commit -m 'new commit message'

    #correct message and updates files:

      git commit -m 'new commit message'

##push

  #pushing is updating the chages you made

  #it is better to add a remote

    R=git@github.com:cirosantilli/bash.git
    B=fli3h
    git push "$R" "$B"

    git push -u origin master
    git push

  #``-u``: git remembers last push. next time, no needto repeat params.

##tags

  #are names to commits. usefull for versioning

    git tag "v1.3"
    git push --tags

##fetch

  #get changes from remote

  #*does not change* current repo

  #creates new remote/branch branches

    git remote add upstream git@github.com:cirosantilli/test.git
    git fetch upstream
    git branch -r

  #origin/master

  #upstream/master

##pull

    git pull origin master
  #takes changes from the origin repo to current repo
    #if there are conflicts, you will have to merge them

##move

  #move OLD_PATH to NEW_PATH.

    git mv "$OLD_PATH" "$NEW_PATH"

  #if the new path already exists, nothing is done. use ``-f`` to force overwrite.

  #if the new path exists, it is overwritten:

    git mv -f "$OLD_PATH" "$NEW_PATH"

  #if moving would lead to an error (overwrite without -f or file not tracked), skip the move:

    git mv -k "$OLD_PATH" "$NEW_PATH"

##rm

  #stop tracking file and permanently delete it on disk:

    git rm "$PATH"

  #only works if the files has not been modified after last commit. If it has, you must use the -f option.
  #if the file exist but is not being tracked, nothing is done

  #stop tracking file, but keep source:

    git rm --cached "$PATH"

  #rm even if the file has been modified after last commit
  
    git rm -f "$PATH"

  #thus possible resulting is loss of data
  #if the file is not being tracked, nothing is done

  #recurse remove into directories:

    git rm -r "$PATH"

  #removes only tracked files that were mannualy deleted:

    git ls-files --deleted -z | xargs -0 git rm

  #this might be useful since if you try to remove an untracked file with git, nothing happens

  #RECURSIVE remove:

    git rm '*.txt'

##time travel

  #referring to commits

  #- master :
  #- start of hash (4,6 chars, such as 1ab4)

    #to find the hashes:

      git log	#see hashes of versions
      git log -p $file #history for a given file
      git log --follow -p file #idem, but also goes back on renames

  #- HEAD: most recent
  #- HEAD^: before most recent
  #- HEAD^^: two before most recent
  #- HEAD~2: two before most recent
  #- tag: version tag, usually versions such as v.1.2.

  ##diff
    
    #see differences between two commits:

      git diff $C1 $C2

    #on a single file:

      git diff $C1 $C2 $F

    #staged differences before commit.

      git diff --cached "$F"

    #only for files that have been ``git add``.

      git diff HEAD "$F"
    #all differences between current state and last commit
      #no need to have added the files

    git checkout $hash  #go back to given Hash and KEEP changes. new commits will start branches
    git checkout master #go back to last commit and lose uncommited changes on all files
    git checkout $hash  #go back to hash (beginning of hash). lose uncommited
    git checkout $hash -- $file1 $file2

  #revert only those files
  #if they had different names/location back then, you must put the old names
  #  and they will be restored under the old names/location

    git reset --hard	#go back to last commit and lose all changes made afterwards
    git reset --hard $hash #go back to hashe's commit and lose all that comes after it

  ##remote branches

    #you cannot revert history on remote branches without risking to destroy people's work!

    #must to a correcting commit. revert command for that

    #if you are sure you are the only one, push --force will do

##branch

  #branching is creating a separate path of developement

  #show branches. * before current:

    git branch

  #shows branches from all remotes as remote/branch pairs:

    git branch -r

  #create a branch:

    git branch "$B"

  #go to that branch:

    git checkout "$B"

  #create a branch an go to it:

    git checkout -b "$B"

  #delete branch:

    git branch -d "$B"

##merge

  #files get modified to contain trash merge markers

  #open with a merging tool:

    git mergetool

##submodule

  #use outside repos inside you main repo,

  #if you modify the outside repo, it is easy to push it back to its origin by itself!
  
  #commands outside of the submodule (log, diff, etc) don't see what goes on inside it

  #commands inside it do!

  #add new submodule to project

    git submodule add git://github.com/chneukirchen/rack.git rack

  #add an external submodule to dir named rack

  #clone project with submodule:

    git clone git://github.com/schacon/myproject.git #submodule is empty
    git submodule init #create .gitmodule
    git submodule update #get module from server

  ##remove

    #Delete the relevant section from the .gitmodules file:

      vim .submodules

    #delete the relevant section from .git/config:

      vim .git/config

      rm --cached $path_to_submodule #(no trailing slash).

      rm -Rf .git/modules/$path_to_submodule

      git commit -am 'removed submodule'

      rm -rf $path_to_submodule

##permissions

  #git keeps file permissions (rwx) as metadata inside the ``.git`` dir

##symlinks

  ##on push

    #git stores symlinks as files containing the link location
    #+ some metadata inside ``.git`` that indicates that it is a symlink.

  ##on pull

    #git recreates the symlinks on local system

  #http://stackoverflow.com/questions/954560/what-does-git-do-to-files-that-are-a-symbolic-link

    echo b > a
    ln -s  a aln
    git add a aln
    git commit -am 'link'
    git push master origin

##.gitkeep

  #git ignores empty dirs

  #this is a conventional placeholder filename to force git to keep files.

  #it has no special meaning to git and is not documented.

  #possible better alternative: use a ``readme`` explaining the purpose of the dir!

##github api v3 via curl

    USER=cirosantilli
    REPO=repo
    PASS=

  #get repo info (large):

    curl -i https://api.github.com/users/$USER/repos

  #create remote git repo:

      curl -u "$USER" https://api.github.com/user/repos -d '{"name":"'$REPO'"}'
      git remote add github git@github.com:$USER/$REPO.git
      git push origin master

    #{
      #"name": "$REPO",
      #"description": "This is your first repo",
      #"homepage": "https://github.com",
      #"private": false,
      #"has_issues": true,
      #"has_wiki": true,
      #"has_downloads": true
    #}

  #delete repo:

      curl -u "$USER" -X DELETE https://api.github.com/repos/$USER/$REPO

  #careful, it works!!!

##svn tutorial

  #didn't expect this here did you?

  #create new svn controlled folder:

    svnadmin create project

  #clone:

    svn checkout https://subversion.assembla.com/svn/cirosantillitest/

  #clone version number 3:

    svn checkout -r 3 file:///home/user/svn project

  #everything you commit increases this number

  #take a non svn file directory and convert it to svn controlled one:

    svn import nonsvn svn

  #commit. if your ssh is added, this pushes to the original repository!!!
  #this is why in svn everything happens over the network!!

    svn commit -m 'commit message'

  #history of commits:

    svn log

  #what is commited, changed and tracked:

    svn status

  #must use to make dirs, -m to commit with message:

    svn mkdir foo -m 'commit message'

  #must use to remove dirs, m to commit with message:

    svn rmdir foo -m 'commit message'

  #add to version controll:

    svn add

  ##get single file from repo, modify it, and up again

      svn co https://subversion.assembla.com/svn/cirosantillitest/ . --depth empty
      svn checkout readme.textile
      vim readme.textile
      svn ci -m 'modified readme.textile'
