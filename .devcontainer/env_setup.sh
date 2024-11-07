# https://www.reddit.com/r/bash/comments/3gu16u/whats_your_favourite_ps1_prompt/

# Get Git branch of current directory
git_branch () {
    if git rev-parse --git-dir >/dev/null 2>&1
        then echo -e "" git:\($(git branch 2>/dev/null| sed -n '/^\*/s/^\* //p')\)
    else
        echo ""
    fi
}

# Set a specific color for the status of the Git repo
git_color() {
    local STATUS=`git status 2>&1`
    if [[ "$STATUS" == *'Not a git repository'* ]]
        then echo "" # nothing
    else
        if [[ "$STATUS" != *'working directory clean'* ]]
            then echo -e '\033[0;31m' # red if need to commit
        else
            if [[ "$STATUS" == *'Your branch is ahead'* ]]
                then echo -e '\033[0;33m' # yellow if need to push
            else
                echo -e '\033[0;32m' # else green
            fi
        fi
    fi
}

export PS1='\[\033[0;34m\]\u\[\033[0;35m\] at \[\033[0;34m\]\h\[\033[0;35m\] → \[\033[0;30m\]\e[46m[\w]\e[0m$(git_color)$(git_branch)\n\[\033[0;34m\]\$ '