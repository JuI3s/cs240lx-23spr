#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

enum opcode
{
    OPCODE_BRANCH = 'B',
    OPCODE_KILL = 'K',
    OPCODE_DEREF = 'D',
};

struct exprmap
{
    uint64_t *exprs, *deref_labels;
    size_t n_exprs;
};

inline static void display_exprmap(struct exprmap *entry)
{
    printf("Display exprmap, n_exprs: %d\n", entry->n_exprs);
    for (int i = 0; i < entry->n_exprs; i++)
    {
        printf("(%lu, %lu) ", entry->exprs[i], entry->deref_labels[i]);
    }
    printf("\n");
}

inline static struct exprmap make_empty_exprmap_fixed_size(size_t size)
{
    struct exprmap ret =
        {
            .deref_labels = malloc(size * sizeof(uint64_t)),
            .exprs = malloc(size * sizeof(uint64_t)),
            .n_exprs = size

        };
    return ret;
}

// each of these should be ~5-10loc for a basic implementation. if you want to
// go crazy, try keeping it sorted & using binary search (only once you have
// everything working initially!).
uint64_t lookup_exprmap(const struct exprmap set, uint64_t expr)
{
    // you can return 0 if you don't find anything
    // assert(!"unimplemented");

    for (int i = 0; i < set.n_exprs; i++)
    {
        if (set.exprs[i] == expr)
        {
            return set.deref_labels[i];
        }
    }

    return 0;
}

struct exprmap insert_exprmap(const struct exprmap old, uint64_t expr, uint64_t deref_label)
{
    // assert(!"unimplemented");

    printf("c1\n");

    // Check if already exists.
    if (lookup_exprmap(old, expr))
    {
        printf("Already exists\n");
        return old;
    }

    printf("c2\n");

    size_t n_exprs = old.n_exprs + 1;

    struct exprmap ret = make_empty_exprmap_fixed_size(n_exprs);

    for (int i = 0; i < n_exprs - 1; i++)
    {
        ret.deref_labels[i] = old.deref_labels[i];
        ret.exprs[i] = old.exprs[i];
    }

    printf("c3\n");

    ret.deref_labels[n_exprs - 1] = deref_label;
    ret.exprs[n_exprs - 1] = expr;
    return ret;
}

struct exprmap remove_exprmap(const struct exprmap old, uint64_t expr)
{
    // assert(!"unimplemented");
    if (!lookup_exprmap(old, expr))
        return old;

    size_t n_exprs = old.exprs - 1;
    size_t idx = 0;

    struct exprmap ret = make_empty_exprmap_fixed_size(n_exprs);

    for (int i = 0; i < old.exprs; i++)
    {
        if (old.exprs[i] == expr)
            continue;
        ret.deref_labels[idx] = old.deref_labels[i];
        ret.exprs[idx] = old.exprs[i];
        idx++;
    }
    return ret;
}

int subset_exprmap(const struct exprmap small, const struct exprmap big)
{
    // assert(!"unimplemented");

    for (int i = 0; i < small.n_exprs; i++)
    {
        int contained = 0;
        for (int j = 0; j < big.n_exprs; j++)
        {
            if (big.exprs[j] == small.exprs[i])
            {
                contained = 1;
                break;
            }
        }

        if (!contained)
        {
            return 0;
        }
    }

    return 1;
}

struct exprmap intersect_exprmaps(const struct exprmap small, const struct exprmap big)
{
    // assert(!"unimplemented");
    size_t n_exprs = small.n_exprs > big.n_exprs ? small.n_exprs : big.n_exprs;
    struct exprmap ret = make_empty_exprmap_fixed_size(n_exprs);
    size_t n_intersections = 0;

    for (int i = 0; i < small.n_exprs; i++)
    {
        for (int j = 0; j < big.n_exprs; j++)
        {
            if (big.exprs[j] == small.exprs[i])
            {
                ret.exprs[n_intersections] = small.exprs[i];
                ret.deref_labels[n_intersections] = small.deref_labels[i];
                n_intersections++;
                break;
            }
        }
    }

    ret.n_exprs = n_intersections;
    return ret;
}

struct instr
{
    uint64_t label;
    enum opcode opcode;
    uint64_t args[3];
    struct instr *nexts[2];

    int visited;
    struct exprmap always_derefed;
};

inline static void display_instr(struct instr *item)
{
    printf("label: %lu, visited: %d, opcode: %d\n", item->label, item->visited, item->opcode);
    if (item->opcode == OPCODE_BRANCH)
    {
        printf("Branch instr\n");
        printf("Next 0 label: %lu, next 1 label: %lu\n", item->nexts[0]->label, item->nexts[1]->label);
        printf("Arg0: %lu, arg1: %lu\n", item->args[0], item->args[1]);
    }
    else
    {
        printf("Arg: %lu\n", item->args[0]);
        if (item->nexts[0])
        {
            printf("Next 0 label: %lu\n", item->nexts[0]->label);
        }
        else
        {
            printf("end\n");
        }
    }
    display_exprmap(&item->always_derefed);
    printf("---------------------\n");
}

void visit(struct instr *instr, struct exprmap derefed)
{
    // Update instr->visited, instr->always_derefed, derefed, or return as
    // needed for each of the following cases:
    // (1) instr is NULL
    // (2) this is the first path to reach instr
    // (3) along every prior path to @instr every expression in @derefed has
    //     been derefed
    // (4) there are some expressions in instr->always_derefed that are not in
    //     @derefed along this path
    // should be about 10loc total; use the data structure operations you
    // implemented above!
    // assert(!"unimplemented");

    // (1) instr is NULL
    if (!instr)
    {
        printf("Return from null\n");
        return;
    }

    printf("Visiting instr label: %lu\n", instr->label);
    display_instr(instr);

    // (2) this is the first path to reach instr
    if (!instr->visited)
    {
        instr->visited = 1;
        instr->always_derefed = derefed;
    }
    else
    {
        instr->always_derefed = intersect_exprmaps(instr->always_derefed, derefed);
        if (instr->always_derefed.n_exprs == intersect_exprmaps(instr->always_derefed, derefed).n_exprs)
        {
            printf("Return early from instr: %lu\n", instr->label);
            return;
        }
    }

    printf("b1\n");

    // now actually process the instruction:
    // (1) if it's a kill, then we no longer know anything about instr->args[0]
    // (2) if it's a deref, then we should remember that instr->args[0] has
    //     been derefed for the remainder of this path (at least, until it's
    //     killed later on)
    // assert(!"unimplemented");

    if (instr->opcode == OPCODE_KILL)
    {
        derefed = remove_exprmap(derefed, instr->args[0]);
        printf("Remove deref: %d\n", derefed.n_exprs);
    }

    printf("b2\n");

    if (instr->opcode == OPCODE_DEREF)
    {
        printf("DEREF\n");
        derefed = insert_exprmap(derefed, instr->args[0], instr->label);
        printf("INSERT deref: %d\n", derefed.n_exprs);
    }

    printf("b3\n");

    // now recurse on the possible next-instructions. we visit nexts[1] first
    // out of superstition (it's more likely to be NULL and we want to do the
    // most work in the tail recursive call)

    if (instr->opcode == OPCODE_BRANCH)
    {
        visit(instr->nexts[1], derefed);
    }

    printf("b4\n");

    visit(instr->nexts[0], derefed);

    // visit(instr->nexts[1], derefed);
}

void check(struct instr *instr)
{
    if (instr)
    {
        display_instr(instr);
    }
    if (!instr || instr->opcode != OPCODE_BRANCH)
        return;
    if (!lookup_exprmap(instr->always_derefed, instr->args[0]))
        return;
    printf("%lu:%lu\n", lookup_exprmap(instr->always_derefed, instr->args[0]),
           instr->label);
}

int main()
{
    // you don't need to modify any of this; it just parses the input IR, then
    // calls visit on the first instruction, then calls check on every
    // instruction.
    struct instr *head = NULL;
    size_t n_instructions = 0, max_label = 0;
    while (!feof(stdin))
    {
        struct instr *new = calloc(1, sizeof(*new));
        char opcode;
        if (scanf(" %lu %c", &(new->label), &opcode) != 2)
        {
            // if this is failing unexpectedly, and you're on a Mac, maybe just
            // remove it? still seemed to work for Manya
            assert(feof(stdin));
            break;
        }
        new->opcode = opcode;
        int n_args = (new->opcode == OPCODE_BRANCH) ? 3 : 1;
        for (size_t i = 0; i < n_args; i++)
            assert(scanf(" %lu", &(new->args[i])) == 1);
        new->nexts[0] = head;
        head = new;
        n_instructions++;
        if (new->label > max_label)
            max_label = new->label;
        while (!feof(stdin) && fgetc(stdin) != '\n')
            continue;
    }

    size_t n_labels = max_label + 1;

    printf("Num labels: %d\n", n_labels);

    struct instr **label2instr = calloc(n_labels, sizeof(*label2instr));

    for (struct instr *instr = head; instr; instr = instr->nexts[0])
    {
        assert(!label2instr[instr->label]);
        label2instr[instr->label] = instr;
    }

    struct instr *new_head = NULL;
    for (struct instr *instr = head; instr;)
    {
        struct instr *real_next = instr->nexts[0];
        instr->nexts[0] = new_head;
        new_head = instr;
        instr = real_next;
    }
    head = new_head;

    for (size_t i = 0; i < n_labels; i++)
    {
        struct instr *instr = label2instr[i];
        if (instr && instr->opcode == OPCODE_BRANCH)
        {
            if (!(label2instr[instr->args[1]]) || !(label2instr[instr->args[2]]))
            {
                exit(1);
            }
            printf("Next 0 label: %lu\n", instr->args[1]);
            printf("Next 1 label: %lu\n", instr->args[2]);

            instr->nexts[0] = label2instr[instr->args[1]];
            instr->nexts[1] = label2instr[instr->args[2]];
        }
    }

    visit(head, (struct exprmap){NULL, NULL, 0});
    printf("Done visiting\n");
    for (size_t i = 0; i < n_labels; i++)
        check(label2instr[i]);
}
